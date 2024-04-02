# -*- coding: utf-8 -*-

=begin

TECSCDE - TECS Component Diagram Editor

Copyright (C) 2014-2015 by TOPPERS Project

 The above copyright holders grant permission gratis to use,
 duplicate, modify, or redistribute (hereafter called use) this
 software (including the one made by modifying this software),
 provided that the following four conditions (1) through (4) are
 satisfied.

 (1) When this software is used in the form of source code, the above
     copyright notice, this use conditions, and the disclaimer shown
     below must be retained in the source code without modification.

 (2) When this software is redistributed in the forms usable for the
     development of other software, such as in library form, the above
     copyright notice, this use conditions, and the disclaimer shown
     below must be shown without modification in the document provided
     with the redistributed software, such as the user manual.

 (3) When this software is redistributed in the forms unusable for the
     development of other software, such as the case when the software
     is embedded in a piece of equipment, either of the following two
     conditions must be satisfied:

   (a) The above copyright notice, this use conditions, and the
       disclaimer shown below must be shown without modification in
       the document provided with the redistributed software, such as
       the user manual.

   (b) How the software is to be redistributed must be reported to the
       TOPPERS Project according to the procedure described
       separately.

 (4) The above copyright holders and the TOPPERS Project are exempt
     from responsibility for any type of damage directly or indirectly
     caused from the use of this software and are indemnified by any
     users or end users of this software from any and all causes of
     action whatsoever.

 THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
 THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
 PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
 TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
 INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.

 $Id: tmodel.rb 2640 2017-06-03 11:27:12Z okuma-top $

=end

#
# methods marked *** can be called externally.
# don't call unmarked methods other than TECSModel.
#

module TECSCDE

  def self.error( msg )
    puts( msg )
  end

  #== TmObject: base class for TECSModel & its children
  #
  class TmObject
    #@owner::
    #  TmRegion  => TmRegion, TECSModel(for root region)
    #  TmCell    => TmRegion
    #  TmPort    => TmCell | TmCPortArray | TmEPortArray
    #  TmJoin    => TECSModel
    #  TmJoinBar => TmJoin
    #  TECSModel => Nil
    def set_owner owner
      @owner = owner
    end

    def get_owner
      @owner
    end

    def get_model
      if @owner
        return @owner.get_model
      else
        if self.kind_of? TECSModel
          raise "get_model: self is not TECSModel: #{self.class}"
        end
        self
      end
    end

    def modified &proc
      get_model.add_change_set self
      # Ruby3.0: obsolete
      # proc = Proc.new
      proc.call
    end

    def copy_from tm_object
      tm_object.instance_variables.each{ |iv|
        val = tm_object.instance_variable_get( iv )
        if val.kind_of?( Array ) || val.kind_of?( Hash )
          instance_variable_set( iv, val.dup )
        else
          instance_variable_set( iv, val )
        end
      }
    end
  end

  #==ChangeSetControl: provide methods for controling ChangeSet
  module ChangeSetControl
    #== ChangeSet class
    #record each change (change by user's operation)
    class ChangeSet
      def initialize number
        @set = {}
        @number = number
      end

      def add tm_object
        if ! @set.has_key?( tm_object )
          # flush_print "add_change_set #{tm_object.class} number=#{@number}\n"
          @set[ tm_object ] = tm_object.clone_for_undo
        end
      end

      def set_undo_point
        count = @set.length
        flush_print "* set_undo_point number=#{@number}, count=#{count}\n"
        return count
      end

      def apply
        # print "applying change_no=#{@number}\n"
        dbgPrint "applying change_no=#{@number}\n"
        @set.each_key{ |tm_object|
          tm_object.copy_from @set[ tm_object ]
          dbgPrint "apply #{tm_object.class}\n"
        }
      end
    end

    #== ChangeSetMangager class
    #contain all changes & execute undo
    class ChangeSetManager
      def initialize
        @change_no = 0
        @change_set_list = [ ]
        @change_set_next = ChangeSet.new( @change_no )
      end

      #=== ChangeSetManager#add_change_set
      # at the time modifying tm_object, record only the changed tm_object
      def add_change_set tm_object
        # flush_print "add_change_set #{tm_object.class} change_set=#{@change_no}\n"
        @change_set_next.add tm_object
      end

      def set_undo_point
        count = @change_set_next.set_undo_point
        if count > 0
          # flush_print "* set_undo_point change_no=#{@change_no}, count=#{count}\n"
          # p "* set_undo_point change_no=#{@change_no}, count=#{count}\n"
          @change_set_list[ @change_no ] = @change_set_next
          @change_no += 1
          @change_set_next = ChangeSet.new( @change_no )

          if @change_set_list.length > @change_no
            flush_print "truncate undo buffer #{@change_set_list.length} to #{@change_no}\n"
            # print( "range: #{(@change_no)..(@change_set_list.length)-1}\n" )
            # p "length0=#{@change_set_list.length}"
            @change_set_list.slice!( (@change_no)..(@change_set_list.length-1) )
            # p "length1=#{@change_set_list.length}"
          end
        else
          flush_print "* set_undo_point: nothing changed\n"
        end
      end

      # assumed undo is done just after set_undo_point (this means @change_set_next has no contents)
      def undo
        if @change_no > 1
          @change_no -= 1
          flush_print "* undo change_no=#{@change_no}\n"
          @change_set_list[ @change_no ].apply
          # flush_print "* undo1 change_no=#{@change_no}\n"
          @change_set_next = ChangeSet.new( @change_no )
        end
      end

      def redo
        if @change_set_list.length > @change_no + 1
          @change_no += 1
          flush_print "* redo change_no=#{@change_no}\n"
          @change_set_list[ @change_no ].apply
        end
      end
    end # class ChangeSetManger

    #----- ChangeSetControl methods -----#
    def init_change_set
      @change_set_manager = ChangeSetManager.new
    end

    #=== ChangeSetControl#add_change_set
    # at the time modifying tm_object, record only the changed tm_object
    def add_change_set tm_object
      # flush_print "add_change_set #{tm_object.class} change_set=#{@change_no}\n"
      @change_set_manager.add_change_set tm_object
    end

    def set_undo_point
      @change_set_manager.set_undo_point
    end

    def undo
      @change_set_manager.undo
    end

    def redo
      @change_set_manager.redo
    end
  end

  class TECSModel < TmObject

    include ChangeSetControl

    # edges for join (connected by Bars from TmCPort to TmEPort)
    EDGE_TOP    = 0b00
    EDGE_BOTTOM = 0b01
    EDGE_LEFT   = 0b10
    EDGE_RIGHT  = 0b11

    # gap is length between parallel bars.
    CPGap = 10  # (mm)
    EPGap = 10  # (mm)
    Gap   = 5   # (mm)
    ALIGN = 1.0 # (mm)    # grid size

    # acceptable error of position information in .cde file
    MAX_ERROR_IN_NOR = 0.5
    MAX_ERROR_IN_TAN = 2

    # minmal distance to next port (minimal interval)
    DIST_PORT = 4 # (mm)

    # Paper Size w/o margin (10 mm each side)
    Paper = { :A4L => { :size => "A4", :orientation => "LANDSCAPE", :name => "A4_LANDSCAPE", :height => 190, :width => 277 },
              :A3L => { :size => "A3", :orientation => "LANDSCAPE", :name => "A3_LANDSCAPE", :height => 277, :width => 400 },
              :A2L => { :size => "A2", :orientation => "LANDSCAPE", :name => "A2_LANDSCAPE", :height => 400, :width => 574 } }
    # name must  be found in Cairo::Paper

    #
    NEAR_DIST = 2 # (mm)

    IDENTIFIER_RE = /[A-Za-z_][0-9A-Za-z_]*/

    #@paper::Hash : See Paper
    #@cell_list::[TmCell]
    #@cell_hash::{ Symbole(namespace_path) => TmCell }
    #@join_list::[TmJoin]
    #@view::TView
    #@root_region::TmRegion
    #@file_editing::String

    def initialize tecsgen
      @cell_list = []
      @cell_hash = {}
      @join_list = []
      @tecsgen = tecsgen
      @paper = Paper[ :A3L ]
#      @paper = Paper[ :A2L ]

      # __tool_info__( "tecsgen" )
      @direct_import = []
      @import_path_opt = []
      @define    = []
      @cpp       = ""
      @file_editing = "untitled"
      @owner = nil
      init_change_set

      create_root_region
    end

    #=== TECSModel#new_cell ***
    #namespace_path::String : namespace path string of celltype
    def new_cell( xm, ym, celltype_name, ct_namespace_path, tecsgen_cell = nil )

      ct_nsp = NamespacePath.analyze( ct_namespace_path )
      ct_nsp.append! celltype_name.to_sym
      ct = Namespace.find ct_nsp
      if ct == nil
        TECSCDE.error( "TM9999 celltype #{ct_nsp.to_s}: not found for cell #{@name}")
        return
      end

      if tecsgen_cell
        region = get_region_from_tecsgen_region( tecsgen_cell.get_region )
      else
        region = get_region_by_location( xm, ym )
      end
      return new_cell2( xm, ym, ct, region, tecsgen_cell )
    end

    #celltype::Celltype : in tecsgen (should be changed to TmCelltype)
    #region:TmRegion    :
    #tecsgen_cell:Cell  : in tecsgen
    def new_cell2( xm, ym, celltype, region, tecsgen_cell )
      modified {

        name = celltype.get_name.to_s.gsub( /t(.*)/, '\\1' ).to_sym
        if @cell_hash[ name ]
          count = 0
          while @cell_hash[ (name.to_s + count.to_s).to_sym ]
            count +=1
          end
          name = (name.to_s + count.to_s).to_sym
        end

        cell = TmCell.new( name, celltype, xm, ym, region, tecsgen_cell )
        @cell_list << cell
        @cell_hash[ name ] = cell

        w, h = @view.get_text_extent( name, CELL_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
        w2, h = @view.get_text_extent( celltype.get_name, CELL_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
        w += 2
        w = w2 if w2 > w
        w = 20 if w < 20
        h = 13 if h < 13
        cell.set_geometry( xm, ym, w, h )

        return cell
      }
    end

    #=== TECSModel#delete_cell
    # don't call externally, use TmCell#delete instead
    def delete_cell cell
      modified {

        @cell_list.delete cell
        @cell_hash.delete cell.get_name #mikan region
      }
    end

    #=== TECSModel#rename_cell
    #old_name::Symbol
    #cell:: TmCell
    # don't call externally, use TmCell#change_name instead
    def rename_cell( cell, new_name )
      modified {

        if ! new_name.kind_of? Symbol
          raise "cell name not Symbol"
        end
        if cell.get_name == new_name
          return true
        end

        if ! ( new_name =~ IDENTIFIER_RE )
          TECSCDE.message_box( "'#{new_name}' has unsuitable character for identifier", nil )
          return false
        end
        if @cell_hash[ new_name ] then
          TECSCDE.message_box( "'#{new_name}' already exists", nil )
          return false
        end
        @cell_hash.delete cell.get_name
        @cell_hash[ new_name ] = cell
        return true
      }
    end

    #=== TECSModel#new_join ***
    def new_join( cport, eport )
      modified {

        join = TmJoin.new( cport, eport, self )
        @join_list << join
        return join
      }
    end

    #=== TECSModel#delete_join
    # don't call externally. call TmJoin#delete instead
    def delete_join join
      modified {

        @join_list.delete join
      }
    end

    #=== TECSModel.normal direction of edge
    #RETURN:: 1: if direction is positive, -1: negative
    def self.get_sign_of_normal( edge_side )
      ((edge_side & 0b01)) != 0 ? 1 : -1
    end

    #=== TECSModel.is_vertical?
    #RETURN:: true if vertical, false if horizontal
    def self.is_vertical?( edge_side )
      ((edge_side & 0b10) != 0) ? true : false
    end

    #=== TECSModel.is_parallel?
    #RETURN:: true if parallel, false if right anble
    def self.is_parallel?( edge_side1, edge_side2 )
      # p "edge val", edge_side1, edge_side2, edge_side1 ^ edge_side2
      ( edge_side1 ^ edge_side2) < 0b10
    end

    #=== TECSModel.is_opposite?
    # this function can be applicable only when edge_side1, edge_side2 are parallel
    def self.is_opposite?( edge_side1, edge_side2 )
      ((( edge_side1 ^ edge_side2 ) & 0b01 ) !=  0) ? true : false
    end

    #=== TECSModel.round_length_val
    def self.round_length_val val
      round_unit = TECSModel.get_alignment
      # round_unit = 0.25
      # (val / round_unit).round * round_unit
      (val / round_unit).round * round_unit
      # val
    end

    #=== TECSModel#get_cell_list ***
    def get_cell_list
      @cell_list
    end

    #=== TECSModel#get_join_list ***
    def get_join_list
      @join_list
    end

    def get_paper
      @paper
    end

    #=== TECSModel#set_view ***
    def set_view view
      @view = view
    end

    #=== TECSModel#get_celltype_list ***
    def get_celltype_list
      if @tecsgen then
        @tecsgen.get_celltype_list
      end
    end

    #=== TECSModel#get_region_from_tecsgen_region
    def get_region_from_tecsgen_region tecsgen_region
      nsp = tecsgen_region.get_namespace_path
      return get_region_from_namespace_path nsp
    end

    #=== TECSModel#get_region_from_namespace_path
    def get_region_from_namespace_path nsp
      path_array = nsp.get_path
      region = @root_region
      i = 0
      while i < path_array.length
        region = region.get_region path_array[i]
        i += 1
      end
      return region
    end

    #=== TECSModel#get_region_by_location
    def get_region_by_location( x, y )
      @root_region   # mikan
    end

    #=== TECSModel#create_root_region
    def create_root_region
      nsp = NamespacePath.new( "::", true )
      @root_region = TmRegion.new( nsp, self )
    end

    #=== TECSModel#get_file_editing
    #return::String : file name editing
    def get_file_editing
      @file_editing
    end

    #=== TECSModel.get_alignment
    #return::String : file name editing
    def self.get_alignment
      ALIGN
    end

    def clip_x x
      max = @paper[ :width ] - 2
      if x < 2
        x = 2
      elsif x > max
        x = max
      end
      return x
    end

    def clip_y y
      max = @paper[ :height ] - 2
      if y < 2
        y = 2
      elsif y > max
        y = max
      end
      return y
    end

    #=== TECSModel.clone_for_undo
    def clone_for_undo
      bu = clone
      bu.copy_from self
      return bu
    end

    #=== TECSModel.setup_clone
    def copy_from model
      model.instance_variables.each{ |iv|
        val = model.instance_variable_get( iv )
        instance_variable_set( iv, val )
      }
      @cell_list = (model.instance_variable_get :@cell_list).dup
      @cell_hash = (model.instance_variable_get :@cell_hash).dup
      @join_list = (model.instance_variable_get :@join_list).dup
    end

    def get_model
      self
    end

    #-----  TECSModel contents classes -----#

    # class must be descendant of Node
    module TmUneditable
      #@b_editable::Bool | Nil:  objects from .cdl cannot be editable  (used by TmJoin, TmCell)

      #=== TmObject#set_editable
      #locale:: see Node in syntaxobj.rb
      def set_editable locale
        if locale[0] == get_model.get_file_editing
          @b_editable = true
        else
          @b_editable = false
        end
      end

      #=== TmObject#is_editable?  ***
      def is_editable?
        @b_editable
      end
    end

    class TmCell < TmObject
      #@x::Integer
      #@y::Integer
      #@width::Integer
      #@height::Integer
      #@name::Symbol
      #@cports::{ Symbol => CPORT }
      #@eports::{ Symbol => EPORT }
      #@n_cport::Integer
      #@n_eport::Integer
      #@celltype::  ::Celltype
      #@owner::TmRegion
      #@tecsgen_cell::Cell :  not nil if Cell from .cde/.cdl file
      #@attr_list::{Symbol(name)=>Expression}

      include TmUneditable

      def initialize( name, celltype, x, y, region, tecsgen_cell = nil )
        dbgPrint "TmCell.new\n"
        @name = name
        @celltype = celltype
        @owner = region
        @attr_list = {}

        @x = x
        @y = y
        @width  = 25
        @height = 15

        @cports = { }
        @eports = { }
        @n_cport = 0
        @n_eport = 0

        @celltype.get_port_list.each{ |port_def|
          # p "celltype:#{@celltype.get_name} port:#{port_def.get_name}"
          if port_def.get_port_type == :ENTRY then
            #if ! port_def.is_reverse_required? then
            if port_def.get_array_size == nil
              @eports[ port_def.get_name ] = TmEPort.new( self, port_def )
            else
              @eports[ port_def.get_name ] = TmEPortArray.new( self, port_def )
            end
            #end
          else
            if ! port_def.is_require? then
              if port_def.get_array_size == nil
                @cports[ port_def.get_name] = TmCPort.new( self, port_def )
              else
                @cports[ port_def.get_name] = TmCPortArray.new( self, port_def )
              end
            end
          end
        }

        @tecsgen_cell = tecsgen_cell
        @b_editable = true
        modified {
        }
      end

      #=== TmCell#set_geometry
      def set_geometry( x, y, w, h )
        x_inc = x - @x
        y_inc = y - @y
        x_inc_r = x + w - ( @x + @width )
        y_inc_b = y + h - ( @y + @height )

        @cports.each{ |name, cport|
            cport.moved_edge( x_inc, x_inc_r, y_inc, y_inc_b )
        }
        @eports.each{ |name, eport|
            eport.moved_edge( x_inc, x_inc_r, y_inc, y_inc_b )
        }

        w_min, h_min = get_min_wh
        w = w_min if w < w_min
        h = h_min if h < h_min

        @x = TECSModel.round_length_val x
        @y = TECSModel.round_length_val y
        @width = TECSModel.round_length_val w
        @height = TECSModel.round_length_val h
      end

      #=== TmCell#delete ***
      def delete
        if ! is_editable?
          return
        end
        modified {

          @cports.each{ |name, cport|
            cport.delete
          }
          @eports.each{ |name, eport|
            eport.delete
          }
          @owner.delete_cell self
        }
      end

      #=== TmCell#get_geometry ***
      def get_geometry
        [ @x, @y, @width, @height ]
      end

      #=== TmCell#get_name ***
      def get_name
        @name
      end

      #=== TmCell#change_name ***
      #name::Symbol : new name
      #return::Bool: true if succeed
      # if cell of new_name already exists, results false
      def change_name name
        if @owner.rename_cell( self, name )
          modified {
            @name = name
            return true
          }
        else
          return false
        end
      end

      #=== TmCell#get_celltype ***
      def get_celltype
        @celltype
      end

      #=== TmCell#get_region
      #return::TmRegion
      def get_region
        @owner
      end

      #=== TmCell#move ***
      def move x_inc, y_inc
        modified {

          dbgPrint "cell move #{@name}\n"
          x0 = @x; y0 = @y
          @x = get_model.clip_x( TECSModel.round_length_val( @x + x_inc ) )
          @y = get_model.clip_y( TECSModel.round_length_val( @y + y_inc ) )
          x_inc2 = @x - x0; y_inc2 = @y - y0

          @cports.each{ |name, cport|
            cport.moved( x_inc2, y_inc2 )
          }
          @eports.each{ |name, eport|
            eport.moved( x_inc2, y_inc2 )
          }
        }
      end

      #=== TmCell::is_near?( x, y )  ***
      def is_near?( x, y )
        # p "is_near? @x=#{@x} @width=#{@width} @y=#{@y} @height=#{@height} x=#{x} y=#{y}"
        if( @x < x ) &&( x < ( @x+@width) )&&( @y < y )&&( y < (@y+@height) )then
          true
        else
          false
        end
      end

      #=== TmCell::get_near_port ***
      def get_near_port( x, y )
        (@cports.merge @eports).each{ |name, port|
          if port.kind_of? TmPort
            xp, yp = port.get_position
          else
            pt = port.get_near_port( x, y )
            if pt
              return pt
            end
            next
          end
          # p "get_near_port x=#{x} y=#{y} xp=#{xp} yp=#{yp}"
          if((xp-x).abs < NEAR_DIST) && ((yp-y).abs < NEAR_DIST)
            # p "near port: found"
            return port
          end
        }
        return nil
      end

      #=== TmCell#get_edge_position_in_normal_dir
      def get_edge_position_in_normal_dir( edge_side )
        case edge_side
        when  EDGE_TOP
          @y
        when  EDGE_BOTTOM
          @y+@height
        when  EDGE_LEFT
          @x
        when  EDGE_RIGHT
          @x+@width
        end
      end

      #=== TmCell#get_right_angle_edges_position
      def get_right_angle_edges_position( edge_side )
       if TECSModel.is_vertical?( edge_side ) then
          [@y, @y+@height]
       else
          [@x, @x+@width]
        end
      end

      #=== TmCell#inc_n_cport
      # total call port count
      def inc_n_cport
        n = @n_cport
        @n_cport += 1
        return n
      end

      #=== TmCell#inc_n_eport
      # total entry port count
      def inc_n_eport
        n = @n_eport
        @n_eport += 1
        return n
      end

      def get_new_cport_position port_def
        if $b_tate
          return [ EDGE_BOTTOM, DIST_PORT * (inc_n_cport + 1) ]
        else
          return [ EDGE_RIGHT, DIST_PORT * (inc_n_cport + 1) ]
        end
      end
      def get_new_eport_position port_def
        if $b_tate
          return [ EDGE_TOP, DIST_PORT * (inc_n_eport + 1) ]
        else
          return [ EDGE_LEFT, DIST_PORT * (inc_n_eport + 1) ]
        end
      end

      #TmCell#adjust_port_position_to_insert
      #port::TmPort : insert after the port
      def adjust_port_position_to_insert port
        # p "adjust_port_position_to_insert"
        nearest_port = find_nearest_next_port port
        if nearest_port
          dist = ( nearest_port.get_offset - port.get_offset )
          if dist < (DIST_PORT * 2)
            offs = (DIST_PORT * 2) - dist
            adjust_port_position_after_port port, offs
          end
        end
      end

      #TmCell#find_nearest_next_port
      #this method is part of adjust_port_position_to_insert
      def find_nearest_next_port port
        # p "find_nearest_next_port #{port.get_name} #{port.get_subscript}"
        edge_side = port.get_edge_side
        offs = port.get_offset
        proc_judge_near = Proc.new { |port, offs, edge_side, nearest_port|
          # p "find_nearest_next_port: comp: #{port.get_name} #{port.get_subscript} at #{port.get_offset}@#{port.get_edge_side} #{offs}@#{edge_side}"
          if port.get_edge_side == edge_side
            dist = port.get_offset - offs
            # p "dist=#{dist}"
            if dist > 0
              if nearest_port
                if ( nearest_port.get_offset - offs ) > dist
                  nearest_port = port
                end
              else
                nearest_port = port
              end
            end
          end
          nearest_port
        }
        nearest_port = nil
        (@eports.values+@cports.values).each{ |port|
          if port.kind_of? TmPortArray
            port.get_ports.each{ |pt|
              nearest_port = proc_judge_near.call( pt, offs, edge_side, nearest_port )
              # p "nearest=#{nearest_port}"
            }
          else
            nearest_port = proc_judge_near.call( port, offs, edge_side, nearest_port )
            # p "nearest=#{nearest_port}"
          end
        }
        # p "find_nearest=#{nearest_port}"
        return nearest_port
      end

      #=== TmCell#adjust_port_position_after_port port, offs
      #this method is part of adjust_port_position_to_insert
      def adjust_port_position_after_port port, move_offs
        # p "adjust_port_position_after_port"
        edge_side = port.get_edge_side
        offs = port.get_offset
        proc_adjust = Proc.new { |port, offs, edge_side, move_offs|
          if port.get_edge_side == edge_side
            dist = port.get_offset - offs
            if dist > 0
              port.move( move_offs, move_offs )    # move same value for x, y (only x or y applied in the method)
            end
          end
        }
        (@eports.values+@cports.values).each{ |port|
          if port.kind_of? TmPortArray
            port.get_ports.each{ |pt|
              proc_adjust.call( pt, offs, edge_side, move_offs )
            }
          else
            proc_adjust.call( port, offs, edge_side, move_offs )
          end
        }
      end

      #=== TmCell#get_cports ***
      def get_cports
        @cports
      end
      #=== TmCell#get_eports ***
      def get_eports
        @eports
      end

      #=== TmCell#get_cport_for_new_join
      def get_cport_for_new_join cport_name, cport_subscript
        cp = @cports[ cport_name ]
        if cp == nil
          TECSCDE.error( "TM9999 cell #{@name} not have call port #{cport_name}")
        end

        if cport_subscript == nil
          if ! cp.is_array?
            return cp
          else
            TECSCDE.error( "TM9999 cell #{@name}.#{cport_name} is call port array" )
            return nil
          end
        else
          if cp.is_array?
            return cp.get_port_for_new_join( cport_subscript )
          else
            TECSCDE.error( "TM9999 cell #{@name}.#{cport_name} is not call port array" )
            return nil
          end
        end
      end

      #=== TmCell#get_eport_for_new_join
      def get_eport_for_new_join eport_name, eport_subscript
        ep = @eports[ eport_name ]
        if ep == nil
          TECSCDE.error( "TM9999 cell #{@name} not have entry port #{eport_name}")
        end

        if eport_subscript == nil
          if ! ep.is_array?
            return ep
          else
            TECSCDE.error( "TM9999 cell #{@name}.#{eport_name} is entry port array" )
            return nil
          end
        else
          if ep.is_array?
            return ep.get_port_for_new_join( eport_subscript )
          else
            TECSCDE.error( "TM9999 cell #{@name}.#{eport_name} is not entry port array" )
            return nil
          end
        end
      end

      #=== TmCell#set_attr
      #name::Symbol
      #init::String|Nil  (from Expression)
      def set_attr( name, init )
        modified {

          if init == nil
            @attr_list.delete name
          else
            @attr_list[ name ] = init
          end
        }
      end

      def get_attr_list
        @attr_list
      end

      #=== TmCell#complete?
      def complete?
        @celltype.get_attribute_list.each{ |attr|
          if attr.get_initializer == nil
            if @attr_list[ attr.get_name ] == nil
              return false
            end
          end
        }
        @cports.each{ |name, cport|
          if ( ! cport.complete? ) && ( ! cport.is_optional? )
            return false
          end
        }
        return true
      end

      #=== TmCell#get_min_wh
      # minimum width & height of the cell.
      # these values are calculated from ports' offset.
      # name length is not considered.
      def get_min_wh
        h_min = 0
        w_min = 0
        (@cports.values+@eports.values).each{ |port|
          if port.kind_of? TmPortArray
            port.get_ports.each{ |pt|
              offs = pt.get_offset
              case pt.get_edge_side
              when EDGE_TOP, EDGE_BOTTOM
                w_min = offs if offs > w_min
              else
                h_min = offs if offs > h_min
              end
            }
          else
            offs = port.get_offset
            case port.get_edge_side
            when EDGE_TOP, EDGE_BOTTOM
              w_min = offs if offs > w_min
            else
              h_min = offs if offs > h_min
            end
          end
        }
        [ w_min + DIST_PORT, h_min + DIST_PORT ]
      end

      #=== TmCell#get_tecsgen_cell
      def get_tecsgen_cell
        @tecsgen_cell
      end

      #=== TmCell#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end
    end # class TmCell

    class TmRegion < TmObject
      #@sub_region::{name=>TmRegion}

      #=== TmRegion#initialize
      #namespace_path::NamespacePath
      #owner::TmRegion (parent) or TECSModel (root region)
      def initialize( namespace_path, owner )
        @namespace_path = namespace_path
        @owner = owner

        # region's property
        @sub_region = {}
        @cell_list = {}
        modified {
        }
      end

      def get_namespace_path
        @namespace_path
      end

      def get_color
      end

      def delete_cell cell
        @owner.delete_cell cell
      end
      def rename_cell cell, name
        @owner.rename_cell cell, name
      end

      def get_region name
        if @sub_region[name] == nil
          modified {

            parent = self
            @sub_region[name] = TmRegion.new( @namespace_path.append( name ), parent )
          }
        end
        return @sub_region[name]
      end

      #=== TmRegion#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end

      def setup_clone sub_region
        @sub_region = sub_region.dup
      end
    end # class TmRegion

    class TmPortArray < TmObject
      #@ports::[TmPort]
      #@port_def::Port
      #@actual_size::Integer
      #@subscript1::subscript value of 1st element. to check consistency of subscript

      def get_actual_size
        @actual_size
      end

      #TmPortArray#get_port_for_new_join
      #this method is for load
      def get_port_for_new_join( subscript )
        if @subscript1 == nil
          # 1st element of this entry array
          @subscript1 = subscript
        elsif ( @subscript1 >= 0 && subscript < 0 ) || ( @subscript1 < 0 && subscript >= 0 )
          TECSCDE.error( "TM9999 array subscript inconsistent (similar error to S1128)" )
          return nil
        end

        modified {

          # p "new_join: for name:#{@port_def.get_name}[ #{subscript} ] owner:#{@owner.get_name}, len=#{@ports.length}"
          if subscript >= 0
            if subscript >= @actual_size

              # in case of unsized array, extend array
              if @port_def.get_array_size == "[]"
                # extend array size
                (0..subscript).each{ |subsc|
                  if @ports[ subsc ] == nil
                    port = new_port subsc
                    @ports[subsc] = port
                  end
                }
                @actual_size = @ports.length
                # p "new_join: 1 for name:#{@port_def.get_name}[ #{subscript} ] owner:#{@owner.get_name}, len=#{@ports.length}"
                return @ports[ subscript ]
              end

              TECSCDE.error( "#{@owner.get_name}.#{@port_def.get_name}[#{subscript}]: subscript=#{subscript} out of range(0..(#{@actual_size-1})" )
              return nil
            else
              port = @ports[ subscript ]
              # p "new_join: 2 for name:#{@port_def.get_name}[ #{subscript} ] owner:#{@owner.get_name}, len=#{@ports.length}"
              if self.instance_of? TmCPortArray   # CPort cannot have multiple join
                if port.get_join
                  TECSCDE.error( "#{@owner.get_name}.#{@port_def.get_name}[#{subscript}]: duplicate join" )
                  return nil
                end
              end
              return  port
            end
          else # no index
            found = false
            found_port = nil
            @ports.each{ |port|
              if port.get_join == nil
                found = true
                found_port = port
                break
              end
            }
            if found
              return found_port
            end

            # in case of unsized array, extend array
            if @port_def.get_array_size == "[]"
              port = new_port @ports.length
              @ports << port
              @actual_size = @ports.length
              return port
            end
          end
          return nil
        }
      end

      #=== TmPortArray#get_ports
      def get_ports
        @ports
      end

      #=== TmPortArray#get_near_port
      def get_near_port( x, y )
        @ports.each{ |port|
          xp, yp = port.get_position
          # p "get_near_port x=#{x} y=#{y} xp=#{xp} yp=#{yp}"
          if((xp-x).abs < NEAR_DIST) && ((yp-y).abs < NEAR_DIST)
            # p "near port: found"
            return port
          end
        }
        return nil
      end

      #=== TmPortArray#is_array?
      def is_array?
        true
      end

      def moved_edge( x_inc_l, x_inc_r, y_inc_t, y_inc_b )
        @ports.each{ |port|
          port.moved_edge( x_inc_l, x_inc_r, y_inc_t, y_inc_b )
        }
      end

      def moved( x_inc, y_inc )
        @ports.each{ |port|
          port.moved( x_inc, y_inc )
        }
      end

      def get_member subscript
        if subscript < 0 || subscript >= @actual_size
          return nil
        else
          @ports[ subscript ]
        end
      end

      #=== TmPortArray#delete
      # this method is called from TmCell
      def delete
        @ports.each{ |port|
          port.delete
        }
      end

      #=== TmPortArray#delete_hilited
      # this method is called from Control
      def delete_hilited port
        if @port_def.get_array_size != "[]" then
          TECSCDE::message_box( <<EOT, :OK )
Array size is fixed (#{@port_def.get_array_size}).
Cannot delete array member.
EOT
          return
        end
        index = @ports.index port
        if index != 0
          modified {
            p "delete #### subscript=#{port.get_subscript}"
            port.delete
            if @ports.delete( port ) == nil
              p "delete: not found"
            end
            index = 0
            @ports.each{ |port|
              port.set_subscript index
              index += 1
            }
          }
        else
          TECSCDE::message_box( <<EOT, :OK )
cannot delete array member with subscript==0
EOT
        end
      end

      #=== TmPortArray#insert
      # this method is called from Control
      def insert port, before_after
        if @port_def.get_array_size != "[]" then
          TECSCDE::message_box( <<EOT, :OK )
Array size is fixed (#{@port_def.get_array_size}).
Cannot insert array member.
EOT
          return
        end
        modified {

          @owner.adjust_port_position_to_insert port
          subsc = port.get_subscript
          i = @ports.length - 1
          while i > subsc
            @ports[i].set_subscript( @ports[i].get_subscript + 1 )
            @ports[i+1] = @ports[i]
            i -= 1
          end
          new_port = new_port( subsc + 1 )
          new_port.set_position( port.get_edge_side, port.get_offset + DIST_PORT )
          @ports[ subsc + 1 ] =  new_port

          p "insert ####"
        }
      end

      def complete?
        @ports.each{ |port|
          if ! port.complete?
            return false
          end
        }
        return true
      end

      #=== TmPortArray#is_editable?
      def is_editable?
        @owner.is_editable?
      end

      #=== TmPortArray#is_unsubscripted_array?
      def is_unsubscripted_array?
        if @port_def.get_array_size == "[]"
          return true
        else
          return false
        end
      end

      #=== TmPortArray#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end

      def setup_clone ports
        @ports = ports.dup
      end

    end # class TmPortArray

    class TmCPortArray < TmPortArray
      def initialize cell, port_def
        # p "TmCPortArray port_def:#{port_def}"
        @port_def = port_def
        @owner = cell
        if port_def.get_array_size == "[]"
          @actual_size = 1
        else
          @actual_size = port_def.get_array_size
        end

        @ports = []
        (0..(@actual_size-1)).each{ |subscript|
          # p "TmCPortArray: length=#{@ports.length}  subscript=#{subscript}"
          @ports << TmCPort.new( self, port_def, subscript )
        }
        modified {
        }
      end

      def get_join subscript
        if subscript == nil
          return nil
        elsif 0 <= subscript && subscript < @actual_size
          return @ports[ subscript ]
        else
          return nil
        end
      end

      #=== TmCPortArray#complete?
      def complete?
        @ports.each{ |port|
          if ! port.complete?
            return false
          end
        }
        return true
      end

      #=== TmCPortArray#is_optional?
      def is_optional?
        @port_def.is_optional?
      end

      #=== TmCPortArray#new_port
      def new_port subscript
        TmCPort.new( self, @port_def, subscript )
      end
    end # class TmCPortArray

    class TmEPortArray < TmPortArray
      def initialize cell, port_def
        # p "TmEPortArray port_def:#{port_def}"
        @port_def = port_def
        @owner = cell
        if port_def.get_array_size == "[]" then
          @actual_size = 1
        else
          @actual_size = port_def.get_array_size
        end

        @ports = []
        (0..(@actual_size-1)).each{ |subscript|
          @ports << TmEPort.new( self, port_def, subscript )
        }
        modified {
        }
      end

      #=== TmEPortArray#new_port
      def new_port subscript
        TmEPort.new( self, @port_def, subscript )
      end
    end # class TmEPortArray

    class TmPort < TmObject
      #@edge_side::Integer()
      #@offs::Integer(mm)  # distance from top or left side
      #@owner::TmCell | TmXPortArray  (Reverse Reference)
      #@port_def:: ::Port
      #@subscript::Integer | Nil

      #=== TmPort#move
      def move( x_inc, y_inc )
        modified {

          # p "move x=#{x_inc} y=#{y_inc}"
          x, y, w, h = get_owner_cell.get_geometry
          case @edge_side
          when EDGE_LEFT, EDGE_RIGHT
            offs = TECSModel.round_length_val( @offs + y_inc )
            if offs < 0 || offs > h
              return
            end
            x_inc = 0
          when EDGE_TOP, EDGE_BOTTOM
            offs = TECSModel.round_length_val( @offs + x_inc )
            # p "offs=#{offs} x=#{x} w=#{w}"
            if offs < 0 || offs > w
              return
            end
            y_inc = 0
          end
          @offs = offs
          moved_edge( x_inc, x_inc, y_inc, y_inc )
        }
      end

      #=== TmPort#moved_edge
      # moved cell's edge
      #x_inc_l::Float : left edge moved,   value is incremental
      #x_inc_r::Float : right edge moved,  value is incremental
      #y_inc_t::Float : top edge moved,    value is incremental
      #y_inc_b::Float : bottom edge moved, value is incremental
      def moved_edge( x_inc_l, x_inc_r, y_inc_t, y_inc_b )
        case @edge_side
        when EDGE_TOP, EDGE_LEFT
          moved( x_inc_l, y_inc_t )
        when EDGE_BOTTOM
          moved( x_inc_l, y_inc_b )
        when EDGE_RIGHT
          moved( x_inc_r, y_inc_t )
        end
      end

      #=== tmport#get_normal_bar_of_edge
      # (1)  (6) bar from call port. this indicate A position.
      #join::TmJoin
      def get_normal_bar_of_edge join
        pos = get_cell.get_edge_position_in_normal_dir( @edge_side ) + CPGap * TECSModel.get_sign_of_normal( @edge_side )
        TECSModel.is_vertical?( @edge_side ) ? HBar.new( pos, join ) : VBar.new( pos, join )
      end

      #=== TmPort#get_position_in_tangential_dir
      def get_position_in_tangential_dir
        x, y, w, h = get_cell.get_geometry
        ( TECSModel.is_vertical? @edge_side ) ? y + @offs : x + @offs
      end

      def get_position_in_normal_dir
        get_cell.get_edge_position_in_normal_dir( @edge_side )
      end

      def get_sign_of_normal
        TECSModel.get_sign_of_normal @edge_side
      end

      def get_edge_side
        @edge_side
      end

      def get_edge_side_name
        case @edge_side
        when EDGE_TOP
          :EDGE_TOP
        when EDGE_BOTTOM
          :EDGE_BOTTOM
        when EDGE_LEFT
          :EDGE_LEFT
        when EDGE_RIGHT
          :EDGE_RIGHT
        end
      end

      def get_offset
        @offs
      end

      def get_cell
        if @owner.kind_of? TmCell then
          @owner
        else
          @owner.get_owner
        end
      end

      def get_position
        x, y, w, h = get_cell.get_geometry
        case @edge_side
        when EDGE_TOP
          [x+@offs ,y]
        when EDGE_BOTTOM
          [x+@offs ,y+h]
        when EDGE_LEFT
          [x ,y+@offs]
        when EDGE_RIGHT
          [x+w ,y+@offs]
        end
      end

      def get_name
        @port_def.get_name
      end

      #=== TmPort# get_signature
      #RETURN::Signature
      def get_signature
        @port_def.get_signature
      end

      def get_subscript
        @subscript
      end

      def set_subscript subscript
        modified {

          @subscript = subscript
        }
      end

      #=== TmPort#is_array?
      def is_array?
        false
      end

      #=== TmPort#set_position
      def set_position edge_side, offset
        modified {

          @edge_side = edge_side
          @offs = TECSModel.round_length_val offset
        }
      end

      #=== TmPort#delete_hilited
      # delete_hilited if this port is a member of unsubscripted array.
      def delete_hilited
        if ! @owner.is_editable?
          return
        end
        if @owner.kind_of? TmPortArray
          @owner.delete_hilited self
        end
      end

      #=== TmPort#insert
      #before_after::Symbol: :before, :after
      # insert if this port is a member of unsubscripted array.
      def insert before_after
        if @owner.kind_of? TmPortArray
          @owner.insert self, before_after
        end
      end

      #=== TmPort#is_editable?
      def is_editable?
        @owner.is_editable?
      end

      #=== TmPort#get_owner_cell
      def get_owner_cell
        if @owner.kind_of? TmCell
          return @owner
        elsif @owner.kind_of? TmPortArray
          return @owner.get_owner
        else
          raise "unknown cell"
        end
      end
    end # class TmPort

    class TmCPort < TmPort      # mikan cp array
      #@join::TmJoin

      def initialize owner, port_def, subscript = nil
        # p "port_def::#{port_def.get_name}  #{port_def.class}"
        @port_def = port_def
        @owner = owner
        @join = nil
        @subscript = subscript
        # p "subscript=#{subscript}"

        @name = "cCport"  # temporal
        @edge_side, @offs = get_cell.get_new_cport_position port_def
        modified {
        }
      end

      def set_join join
        modified {

          @join = join
        }
      end

      def moved( x_inc, y_inc )
        if @join then
          @join.moved_cport( x_inc, y_inc )
        end
      end

      def get_join subscript = nil
        @join
      end

      #=== TmCPort#delete
      # this method is called from TmCell
      def delete
        if @join
          modified {

            @join.delete
            @join = nil
          }
        end
      end

      #=== TmCPort#delete_join
      # this method is called from TmJoin
      def delete_join
        modified {

          @join = nil
        }
      end

      #=== TmCPort#complete?
      def complete?
        @join ? true : false
      end

      #=== TmCPort#is_optional?
      def is_optional?
        @port_def.is_optional?
      end

      #=== TmCPort#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end

    end # class TmCPort

    class TmEPort < TmPort   # mikan ep array
      #@joins::[TmJoin]

      def initialize owner, port_def, subscript = nil
        @owner = owner
        @port_def = port_def
        @subscript = subscript

        @joins = []
        @edge_side, @offs = get_cell.get_new_eport_position port_def
        modified {
        }
      end

      def moved( x_inc, y_inc )
        @joins.each{ |join|
          join.moved_eport( x_inc, y_inc )
        }
      end

      def add_join join
        modified {

          @joins << join
        }
      end

      #=== TmEPort#include?
      # TmEPort can have plural of joins.
      # test if TmEPort has specified join.
      def include? join
        @joins.include? join
      end

      #=== TmEPort#get_joins
      def get_joins
        @joins
      end

      #=== TmEPort#delete
      # this method is called from TmCell
      def delete
        modified {

          joins = @joins.dup  # in join.edelete delete_join is called and change @joins
          joins.each{ |join|
            join.delete
          }
        }
      end

      #=== TmEPort#delete_join
      # this method is called from TmJoin
      def delete_join join
        modified {
          @joins.delete join
        }
      end

      #=== TmEPort#complete?
      def complete?
        ( @joins.length > 0 ) ? true : false
      end

      #=== TmEPort#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end

      def setup_clone joins
        @joins = joins.dup
      end
    end # class TmEPort

    class TmJoin < TmObject
      #@cport::TmCPort
      #@eport::TmEPort
      #@bars::[HBar|VBar]
      #@owner::TECSModel

      include TmUneditable

      def initialize cport, eport, tmodel
        @cport = cport
        @eport = eport
        @owner = tmodel

        cport.set_join self
        eport.add_join self

        create_bars
        # @bars.each{ |bar| bar.set_join self }

        @b_editable = true
        modified {
        }
      end

      #=== TmJoin#create_bars_to dest_port
      def create_bars
        if TECSModel.is_parallel?( @cport.get_edge_side, @eport.get_edge_side )
          if TECSModel.is_opposite?( @cport.get_edge_side, @eport.get_edge_side )
            create_bars_a
          else
            create_bars_e
          end
        else
          create_bars_c
        end
      end

      #=== TmJoin#create_bars_a
      # (a) parallel opposite side generic
      def create_bars_a
        @bars = []

        @bars[0] = @cport.get_normal_bar_of_edge self

        posa = @cport.get_position_in_tangential_dir
        e1, e2 = @eport.get_cell.get_right_angle_edges_position( @cport.get_edge_side )
        # p "posa=#{posa} e1=#{e1}, e2=#{e2}"
        pos1 = (( posa - e1 ).abs > ( posa - e2 ).abs) ? (e2 + Gap ) : ( e1 - Gap )
        @bars[1] = (@bars[0].instance_of? HBar) ? VBar.new( pos1, self ) : HBar.new( pos1, self )

        pos2 = @eport.get_position_in_normal_dir + EPGap * @eport.get_sign_of_normal
        @bars[2] = (@bars[1].instance_of? HBar) ? VBar.new( pos2, self ) : HBar.new( pos2, self )

        pos3 = @eport.get_position_in_tangential_dir
        @bars[3] = (@bars[2].instance_of? HBar) ? VBar.new( pos3, self ) : HBar.new( pos3, self )

        pos4 = @eport.get_position_in_normal_dir
        @bars[4] = (@bars[3].instance_of? HBar) ? VBar.new( pos4, self ) : HBar.new( pos4, self )
      end

      #=== TmJoin#create_bars_c
      # (c) right angle generic
      def create_bars_c
        @bars = []

        @bars[0] = @cport.get_normal_bar_of_edge self

        pos1 = @eport.get_position_in_normal_dir + EPGap * @eport.get_sign_of_normal
        @bars[1] = (@bars[0].instance_of? HBar) ? VBar.new( pos1, self ) : HBar.new( pos1, self )

        pos2 = @eport.get_position_in_tangential_dir
        @bars[2] = (@bars[1].instance_of? HBar) ? VBar.new( pos2, self ) : HBar.new( pos2, self )

        pos3 = @eport.get_position_in_normal_dir
        @bars[3] = (@bars[2].instance_of? HBar) ? VBar.new( pos3, self ) : HBar.new( pos3, self )
      end

      #=== TmJoin#create_bars_e
      # (e) parallel same side generic
      def create_bars_e
        @bars = []

        @bars[0] = @cport.get_normal_bar_of_edge self

        posa = @cport.get_position_in_tangential_dir
        e1, e2 = @eport.get_cell.get_right_angle_edges_position( @cport.get_edge_side )
        pos1 = (( posa - e1 ).abs > ( posa - e2 ).abs) ? (e2 + Gap ) : ( e1 - Gap )
        @bars[1] = (@bars[0].instance_of? HBar) ? VBar.new( pos1, self ) : HBar.new( pos1, self )

        pos2 = @eport.get_position_in_normal_dir + EPGap * @eport.get_sign_of_normal
        @bars[2] = (@bars[1].instance_of? HBar) ? VBar.new( pos2, self ) : HBar.new( pos2, self )

        pos3 = @eport.get_position_in_tangential_dir
        @bars[3] = (@bars[2].instance_of? HBar) ? VBar.new( pos3, self ) : HBar.new( pos3, self )

        pos4 = @eport.get_position_in_normal_dir
        @bars[4] = (@bars[3].instance_of? HBar) ? VBar.new( pos4, self ) : HBar.new( pos4, self )
      end

      #=== TmJoin#get_ports_bars ***
      def get_ports_bars
        [@cport, @eport, @bars]
      end
      def get_bars
        @bars
      end

      def get_cport
        @cport
      end

      def get_eport
        @eport
      end

      def moved_cport( x_inc, y_inc )
        if @bars[0].instance_of? VBar then
          @bars[0].moved y_inc
        else
          @bars[0].moved x_inc
        end
      end

      def moved_eport( x_inc, y_inc )
        dbgPrint "moved_eport=(#{x_inc} #{y_inc})\n"
        len = @bars.length

        if len >= 5
          if @bars[len-4].instance_of? VBar then
            @bars[len-4].moved y_inc
          else
            @bars[len-4].moved x_inc
          end
        end

        if len >= 4
          if @bars[len-3].instance_of? VBar then
            @bars[len-3].moved y_inc
          else
            @bars[len-3].moved x_inc
          end
        end

        if len >= 3
          if @bars[len-2].instance_of? VBar then
            @bars[len-2].moved y_inc
          else
            @bars[len-2].moved x_inc
          end
        end

        if @bars[len-1].instance_of? VBar then
          @bars[len-1].moved y_inc
        else
          @bars[len-1].moved x_inc
        end
      end

      #=== TmJoin#get_near_bar ***
      def get_near_bar( xm, ym )
        xs, ys = @cport.get_position
        xe = xs; ye = ys
        min_dist = 999999999
        min_bar = nil
        @bars.each{ |bar|
          if bar.instance_of? HBar
            xe = bar.get_position
            if is_between?( xm, xs, xe ) && is_near?( ym, ys )
              dist = (ym-ys).abs
              if dist < min_dist then
                min_dist = dist
                min_bar = bar
              end
            end
          else # VBar
            ye = bar.get_position
            if is_between?( ym, ys, ye ) && is_near?( xm, xs )
              dist = (xm-xs).abs
              if dist < min_dist then
                min_dist = dist
                min_bar = bar
              end
            end
          end
          xs = xe; ys = ye
        }
        return [min_bar, min_dist]
      end

      #=== TmJoin#is_between?
      #RETURN:: true if x is between a & b
      def is_between?( x, a, b )
        if a >= b
          if  b <= x && x <= a
            true
          else
            false
          end
        else
          if  a <= x && x <= b
            true
          else
            false
          end
        end
      end

      #=== TmJoin#is_near
      def is_near?( x, a )
        (x-a).abs < NEAR_DIST
      end

      #=== TmJoin#change_bars bars
      def change_bars bars
        modified {
          @bars = bars
        }
      end

      #=== TmJoin#get_signature ***
      def get_signature
        @cport.get_signature
      end

      #=== TmJoin#delete ***
      def delete
        if ! is_editable?
          return
        end
        modified {

          @cport.delete_join
          @eport.delete_join self
          @owner.delete_join self
        }
      end

      #=== TmJoin#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end
    end # class TmJoin

    class TmJoinBar < TmObject
      #@position::Integer(mm)     # horizontal(x) or vertical(y) position
      #@owner::TmJoin (Reverse Reference)
      def initialize( position, owner_join )
        @position = position
        @owner = owner_join
        modified {
        }
      end

      #=== TmJoinBar#get_position ***
      def get_position
        @position
      end

      def set_position( position )
        modified {

          @position = TECSModel.round_length_val position
        }
      end

      def moved inc
        set_position( @position + inc )
      end

      # def set_join join
      #  @owner = join
      # end

      #=== TmJoinBar#get_join ***
      def get_join
        @owner
      end

      #=== TmJoinBar#move ***
      # actually moving previous next bar
      # 1st bar and last bar can not be moved (necessary cport, eport move)
      def move( x_inc, y_inc )
        modified {

          bar_prev = nil
          bars = @owner.get_bars

          if bars.length >= 1 && bars[ bars.length - 1 ] == self then
            @owner.get_eport.move( x_inc, y_inc )
            return   # last bar
          end

          bars.each{ |bar|
            if bar.equal? self then
              break
            end
            bar_prev = bar
          }

          if bar_prev  # prev_bar is nil if self is 1st bar
            # p "bar_prev exist"
            if bar_prev.instance_of? HBar
              xm = bar_prev.get_position + x_inc
              bar_prev.set_position( get_model.clip_x xm )
            else
              ym = bar_prev.get_position + y_inc
              bar_prev.set_position( get_model.clip_y ym )
            end
          else
            # 1st bar
            @owner.get_cport.move( x_inc, y_inc )
          end
        }
      end

      #=== TmJoinBar#clone_for_undo
      def clone_for_undo
        bu = clone
        bu.copy_from self
        return bu
      end
    end # class TmJoinBar

    class HBar < TmJoinBar
    end

    class VBar < TmJoinBar
    end
  end
end

=begin

Software Design Memo

pattern of lines between cells

(a) parallel opposite side generic
(b) parallel opposite side abbreviated
(c) right angle generic
(d) right angle  abbreviated
(e) parallel same side generic
(f) parallel same side abbreviated

applying abbrviated patterns, there is conditions.

   +-------------+
   |          (f)|---------------------------1+
   |          (d)|-------------------1+       |
   |          (e)|----1+              |       |
   |          (c)|---1+|              |       |
   |          (a)|--1+||              |       |
   |          (b)|-1+|||              |       |
   |         (c)'|-+||||              |       |
   +-------------+ |||||              |       |
                   ||||+2-------------------3+|
                   |||+2---------3+   |      ||
                   ||+2---3+      |   |      ||
                   ||      | +-------------+ ||
                   ||      4 |    V   V    | 4|
                   ||      +-|>           <|-+|
                   |+2-------|>           <|-2+
                   |         |    ^        |
                   |         +-------------+
                   |              |
                   +--------------+

 edge_side
   horizontal
     EDGE_TOP    = 0b00
     EDGE_BOTTOM = 0b01
   vertical
     EDGE_LEFT   = 0b10
     EDGE_RIGHT  = 0b11

 
  bit0: 1 if normal direction is positive, 0 negative
  bit1: 1 if vertical, 0 if horizontal

  TECSModel class method
    get_sign_of_normal( edge_side ) = (edge_side & 0b01) ? 1 : -1
    is_vertical?( edge_side )   = (edge_side & 0b10) ? true : false
    is_parallel?( edge_side1, edge_side2 ) = ( edge_side1 ^ edge_side2 ) < 0b10
    is_opposite?( edge_side1, edge_side2 ) = ( ( edge_side1 ^ edge_side2 ) & 0b01 ) ? true : false
        this function can be applicable only when edge_side1, edge_side2 are parallel

  TmCell#get_edge_position_in_normal_dir( edge_side )
      case edge_side
      when  EDGE_TOP     y
      when  EDGE_BOTTOM  y+height
      when  EDGE_LEFT    x    
      when  EDGE_RIGHT   x+width

  #=== (1)  (6) bar from call port. this indicate A position.  
  TmCPort#get_normal_bar_of_edge
      pos = @cell.get_edge_position_in_normal_dir( @edge_side ) + Gap * TECSModel.get_sign_of_normal( @edge_side )
      TECSModel.is_vertical?( @edge_side ) ? HBar.new( pos ) : VBar.new( pos )

  TmCPort#tangential_position
      ( TECSModel.is_vertical? @edge_side ) ? @cell.get_y + @offs : @cell.get_x + @offs

  TmJoin#create_bars
      if TECSModel.is_parallel?( @edge_side, dest_port.get_edge_side )
          if TECSModel.is_opposite?( @edge_side, dest_port.get_edge_side )
              create_bars_a
          else
              create_bars_e
      else
          create_bars_c

  TmJoin#create_bars_a
       @bars = []

       @bars[0] = @cport.get_normal_bar_of_edge

       posa = @cport.get_position_in_tangential_dir
       e1, e2 = @eport.get_cell.get_right_angle_edges_position( @cport.get_edge_side )
       pos2 = ( posa - e1 ).abs > ( posa - e2 ).abs ? e2 : e1
       @bars[2] = (bar[1].instance_of? HBar) ? VBar.new( pos2 ) : HBar.new( pos2 )
       
       pos3 = @eport.get_position_in_normal_dir + Gap * @eport.get_sign_of_normal
       @bars[2] = (@bars[1].instance_of? HBar) ? VBar.new( pos3 ) : HBar.new( pos3 )

       pos4 = @eport.get_position_in_normal_dir + Gap * @eport.get_sign_of_normal
       @bars[3] = (@bars[2].instance_of? HBar) ? VBar.new( pos4 ) : HBar.new( pos4 )

       pos5 = @eport.get_position_in_tangential_dir
       @bars[4] = (@bars[3].instance_of? HBar) ? VBar.new( pos5 ) : HBar.new( pos5 )

       pos6 = @eport.get_position_in_normal_dir
       @bars[5] = (@bars[4].instance_of? HBar) ? VBar.new( pos6 ) : HBar.new( pos6 )

  TmJoin#create_bars_c
       @bars = []

       @bars[0] = @cport.get_normal_bar_of_edge

       pos1 = @eport.get_position_in_normal_dir + Gap * @eport.get_sign_of_normal
       @bars[1] = (bar[0].instance_of? HBar) ? VBar.new( pos1 ) : HBar.new( pos1 )
       
       pos2 = @eport.get_position_in_tangential_dir
       @bars[2] = (bar[1].instance_of? HBar) ? VBar.new( pos2 ) : HBar.new( pos2 )

       pos3 = @eport.get_position_in_normal_dir
       @bars[3] = (bar[2].instance_of? HBar) ? VBar.new( pos3 ) : HBar.new( pos3 )

  TmJoin#create_bars_e
       @bars = []

       @bars[0] = @cport.get_normal_bar_of_edge

       pos1 = @eport.get_position_in_normal_dir + Gap * @eport.get_sign_of_normal
       @bars[1] = (bar[0].instance_of? HBar) ? VBar.new( pos1 ) : HBar.new( pos1 )

       posa = @cport.get_position_in_tangential_dir
       e1, e2 = @eport.get_cell.get_right_angle_edges_position( @cport.get_edge_side )
       pos2 = ( posa - e1 ).abs > ( posa - e2 ).abs ? e2 : e1
       @bars[2] = (bar[1].instance_of? HBar) ? VBar.new( pos2 ) : HBar.new( pos2 )

       pos3 = @eport.get_position_in_normal_dir + Gap * @eport.get_sign_of_normal
       @bars[3] = (bar[2].instance_of? HBar) ? VBar.new( pos3 ) : HBar.new( pos3 )

       pos4 = @eport.get_position_in_normal_dir
       @bars[4] = (bar[3].instance_of? HBar) ? VBar.new( pos4 ) : HBar.new( pos4 )



#----- JSON schema (likely) -----#

=end
