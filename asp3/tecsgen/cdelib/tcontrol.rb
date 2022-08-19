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

 $Id: tcontrol.rb 2640 2017-06-03 11:27:12Z okuma-top $

=end

module TECSCDE

=begin

Structure of Palette Window

 +-- @window -----------------------------+
 |+-- @box ------------------------------+|
 ||+- mode (@mode_frame) ---------------+||
 |||+-@mode_vbox-----------------------+|||
 ||||  Pointer  (@pointer_button)      ||||
 |||+----------------------------------+|||
 ||||  New Cell (@new_button)          ||||
 |||+----------------------------------+|||
 ||+- celltypes (@celltype_frame)-------+||
 |||+- ClltypeTreeView&ScrolledWindow--+|||
 |||| name    | region                 ||||
 |||+---------+------------------------+|||
 ||||         |                        ||||
 ||||         |                        ||||
 |||+---------+------------------------+|||
 ||+- cell properties (@mode_celltype) -+||
 |||+- AttrTreeView&ScrolledWindow-----+|||
 |||| name    | type    | value        ||||
 |||+---------+---------+--------------+|||
 ||||         |         |              ||||
 ||||         |         |              ||||
 |||+---------+---------+--------------+|||
 |+--------------------------------------+|
 +----------------------------------------+
=end

  UNSELECTED_STR = "(unselected)"

  class Control
    #@window:: Gtk::Window
    #@model::Model
    #@view::View
    #@mode::Symbol: :NEW_CELL, :POINTER
    #@cport_joining::TmCPort   # :SM_JOINING starting cell
    #@celltypeTreeView::CelltypeTreeView
    #@attrTreeView::AttrTreeView
    #@prev_time::Integer: event time (milli second)

    ModeList    = [ :MODE_NONE, :MODE_NEW_CELL, :MODE_POINTER ]
    SubmodeList = [ :SM_NONE,   :SM_JOINING,    :SM_SURROUNDING_CELLS,
                    :SM_MOVING_CELL_BAR, :SM_MOVING_CPORT, :SM_MOVING_EPORT, :SM_MOVING_CELL_EDGE,
                    :SM_EDIT_CELL_NAME ]

    def initialize model
      @nest = -1
      @model = model
      @hilite_objs = Hilite_objs.new
      @mode     = :MODE_NONE
      @sub_mode = :SM_NONE
      @cport_joining = nil
      @prev_time = 0

      create_new_operation_window
      add_celltype_list

      @hilite_objs.set_attrTreeView @attrTreeView, @cell_name_entry, @cell_region_entry, @cell_frame
      @hilite_objs.update_attrTreeView

      @last_xm = @last_ym = 0
    end

    #----- operations for palette -----#
    def on_save
      flush_print "save\n"; @model.save( @model.get_file_editing )
    end
    def on_export
      fname = @model.get_file_editing.sub( /\.[Cc][Dd][Ee]\Z/, ".pdf" )
      if ! ( fname =~ /\.pdf\Z/ )
        fname += ".pdf"
      end
      flush_print "export to #{fname}\n"
      @view.export( fname )
    end
    def on_pointer
      flush_print "mode: pointer\n"
      @mode = :MODE_POINTER
    end
    def on_new_cell
      @mode = :MODE_NEW_CELL
      flush_print "mode: new\n"
    end
    def on_undo
      @model.undo
      @hilite_objs.reset
      update
    end
    def on_redo
      @model.redo
      @hilite_objs.reset
      update
    end
    def on_quit
      flush_print "quit\n";Gtk.main_quit
    end
    def on_cell_name_entry_active entry
      @b_cell_renaming = true
      @hilite_objs.change_cell_name entry.text
      @b_cell_renaming = false
      update
    end
    def on_cell_name_entry_focus_out entry
      # to avoid nested message box dialog in error case
      if ! @b_cell_renaming
        @hilite_objs.change_cell_name entry.text
        update
      end
    end
    def on_cell_region_entry_active entry
#      @b_cell_renaming = true
#      @hilite_objs.change_cell_name entry.text
#      @b_cell_renaming = false
#      update
    end
    def on_cell_region_entry_focus_out entry
      # to avoid nested message box dialog in error case
#      if ! @b_cell_renaming
#        @hilite_objs.change_cell_name entry.text
#        update
#      end
    end
    def set_attrOperationWidgets window, ctv, attrTreeView, cell_name_entry, cell_region_entry, cell_frame
      @window = window
      @celltypeTreeView = ctv
      @attrTreeView, @cell_name_entry, @cell_region_entry, @cell_frame = attrTreeView, cell_name_entry, cell_region_entry, cell_frame
      @hilite_objs.set_attrTreeView @attrTreeView, @cell_name_entry, @cell_region_etnry, @cell_frame
    end

    #----- palette -----#
    def  create_new_operation_window
      @palette = TECSCDE::Palette.new self
#      @palette.get_entry_cell_name
 #     @palette.get_attrTreeView
    end

    #----- end of palette operations -----#

    def set_view view
      @view = view
      @attrTreeView.set_view view

      # keep controlWindow above mainWindow
      @window.set_transient_for( @view.get_window )
      @window.window.set_group @view.get_window.window
      @window.window.raise

      @palette.set_view view
    end

    #----- canvas events action -----#

    #=== mouse pressed on canvas
    #button::Integer: mouse button number
    #state::GdkModifierType: modifier key state
    #time::Integer: milli second
    #click_count::Integer: 1=single click, 2=double click
    def pressed_on_canvas xm, ym, state, button, time, click_count
      # p "button=#{button} state=#{state} time=#{time} sub_mode=#{@sub_mode}"
      if @sub_mode == :SM_EDIT_CELL_NAME
        name = @view.end_edit_name
        # p "end_edit_name name=#{name}"
        @hilite_objs.change_cell_name name
        @sub_mode = :SM_NONE
      end

      if button == 1
        object = find_near xm, ym
        if object.kind_of?( TECSModel::TmCell ) && click_count == 2
          if object.is_editable?
# p "begin_edit_name"
            @view.begin_edit_name object, time
            @hilite_objs.reset( object )
            @sub_mode = :SM_EDIT_CELL_NAME
          end
        elsif object.kind_of?( TECSModel::TmCell ) ||
#         if object.kind_of?( TECSModel::TmCell ) ||
            object.kind_of?( TECSModel::TmJoinBar )
          @sub_mode = :SM_MOVING_CELL_BAR
          # p "FOUND Cell or Bar"
          if state.shift_mask?
            @hilite_objs.add( object )
          elsif state.control_mask?
            @hilite_objs.add_del( object )
          elsif ! @hilite_objs.include? object
            @hilite_objs.reset( object )
          end
          @view.draw_hilite_objects @hilite_objs
        elsif object.kind_of? TECSModel::TmCPort
          # p "FOUND TmCPort"
          if state.shift_mask?
            @sub_mode = :SM_MOVING_CPORT
            @hilite_objs.add object
          elsif state.control_mask?
            @sub_mode = :SM_MOVING_CPORT
            @hilite_objs.reset( object )
          elsif object.get_join == nil
            @sub_mode = :SM_JOINING
            @hilite_objs.reset
            @cport_joining = object
            @view.set_cursor CURSOR_JOINING
          else
            TECSCDE::message_box( <<EOT, :OK )
Call port has already been joined.
Delete existing join before creating new join.
If you want to hilited port, click with pressing shift key.
EOT
          end
        elsif object.kind_of? TECSModel::TmEPort then
          if state.shift_mask?
            @sub_mode = :SM_MOVING_EPORT
            @hilite_objs.add object
          elsif state.control_mask?
            @sub_mode = :SM_MOVING_EPORT
            @hilite_objs.add_del( object )
          else
            # p "FOUND TmEPort"
            @sub_mode = :SM_MOVING_EPORT
            @hilite_objs.reset object
          end
        else
          # p "NOT FOUND"
          if @mode == :MODE_NEW_CELL then
            ctn, nsp = @celltypeTreeView.selected
            if ctn then
              cell = @model.new_cell( xm, ym, ctn, nsp )
              @model.set_undo_point
            end
            @hilite_objs.reset cell
          else
            @hilite_objs.reset
          end
        end
        @last_xm, @last_ym = xm, ym
      end # button
      @prev_time = time
    end

    #=== mouse moved on canvas
    def motion_on_canvas xm, ym, state
      x_inc = xm - @last_xm
      y_inc = ym - @last_ym

      q, r = x_inc.divmod TECSModel.get_alignment
      x_inc2 = TECSModel.get_alignment * q
      @last_xm = xm - r

      q, r = y_inc.divmod TECSModel.get_alignment
      y_inc2 = TECSModel.get_alignment * q
      @last_ym = ym - r

      case @sub_mode
      when :SM_MOVING_CELL_BAR
        # p "move hilite obj"
        @hilite_objs.each{ |cell_bar|
          cell_bar.move( x_inc2, y_inc2 )
        }
        @view.refresh_canvas
        @view.draw_hilite_objects @hilite_objs
      when :SM_MOVING_CPORT, :SM_MOVING_EPORT
        @hilite_objs.each{ |port|
          port.move( x_inc2, y_inc2 )
        }
        update
        @view.refresh_canvas
        @view.draw_hilite_objects @hilite_objs
      when :SM_JOINING
        object = find_near xm, ym
        if object.kind_of? TECSModel::TmEPort then
          if object.get_signature == @cport_joining.get_signature
            @view.set_cursor CURSOR_JOIN_OK
          end
         # update
        end

      when :SM_NONE
        object = find_near xm, ym
        if object.kind_of? TECSModel::TmCPort then
          @view.set_cursor CURSOR_PORT
        else
          @view.set_cursor CURSOR_NORMAL
        end
      end
    end

    #=== mouse released on canvas
    def released_on_canvas xm, ym, state, button
      case @sub_mode
      when :SM_MOVING_CELL_BAR
        # update
        @model.set_undo_point
      when :SM_MOVING_CPORT, :SM_MOVING_EPORT
        # update
        @model.set_undo_point
      when :SM_JOINING
        object = find_near xm, ym
        if object.kind_of? TECSModel::TmEPort then
          if object.get_signature == @cport_joining.get_signature
            join = @model.new_join( @cport_joining, object )
            @model.set_undo_point
          end
         # update
        end
      end
      @view.set_cursor CURSOR_NORMAL
      if @sub_mode != :SM_EDIT_CELL_NAME
        update
        @sub_mode = :SM_NONE
      end
    end

    def key_pressed keyval, state
      if @sub_mode == :SM_EDIT_CELL_NAME
        
        return
      end

      case keyval
      when 0xff     # delete key
        @hilite_objs.each{ |object|
          if object.kind_of? TECSModel::TmJoinBar
            object.get_join.delete
          elsif object.kind_of? TECSModel::TmCell
            object.delete
          elsif object.kind_of? TECSModel::TmPort
            object.delete_hilited
          end
        }
        @hilite_objs.reset
      when 0x63     # Insert
        @hilite_objs.each{ |object|
          if object.kind_of? TECSModel::TmPort
            object.insert( state.shift_mask? ? :before : :after )
          end
        }
      when 0x51, 0x52, 0x53, 0x54
        case keyval
        when 0x51     # left arrow
          x_inc = - TECSModel.get_alignment; y_inc = 0
        when 0x52     # up arrow
          x_inc = 0.0; y_inc = - TECSModel.get_alignment
        when 0x53     # right arrow
          x_inc = TECSModel.get_alignment; y_inc = 0
        when 0x54     # down arrow
          x_inc = 0.0; y_inc = TECSModel.get_alignment
        end
        @hilite_objs.each{ |obj|
          obj.move( x_inc, y_inc )
        }
      when 0x50     # home
      when 0x57     # end
      when 0x55     # PageUp
      when 0x56     # PageDown
      else
        # printf "key_pressed: keyval=%02x\n", keyval
      end
      if @sub_mode != :SM_EDIT_CELL_NAME
        update
      end
      @model.set_undo_point
    end

    #=== find_near object
    #RETURN::TmCell, TmPort, TmJoin
    def find_near xm, ym
      @model.get_cell_list.each{ |cell|
        port = cell.get_near_port( xm, ym )
        if port != nil then
          # p "found port"
          return port
        end

        if cell.is_near?( xm, ym ) then
          # p "found cell"
          return cell
        end
      }

      # find nearest bar
      min_dist = 999999999
      min_bar = nil
      @model.get_join_list.each{ |join|
        bar, dist = join.get_near_bar( xm, ym )
        if dist < min_dist then
          min_dist = dist
          min_bar = bar
        end
      }
      return min_bar
    end

    #Control#get_hilite_objs
    #return::hilite_objs
    def get_hilite_objs
      @hilite_objs
    end

    def add_celltype_list
      ctl = @model.get_celltype_list
      if ctl then
        ctl.each{ |ct|
          @celltypeTreeView.add ct
        }
      end
    end

    #Control#update
    def update
      @hilite_objs.update_attrTreeView
      @view.paint_canvas
    end

  end # class Control

  #== CelltypeTreeView: show celltype list
  # formerly this class is sub-class of Gtk::TreeView
  # currently this class has Gtk::TreeView
  class CelltypeTreeView

    COL_NAME   = 0
    COL_NSPATH = 1

    #=== initialize
    def initialize treeView
      @treeView = treeView

      # create data model
      liststore = Gtk::ListStore.new(String, String)

      # set data model to tree view(self)
      @treeView.set_model(liststore)
   
      # create renderer for text
      renderer = Gtk::CellRendererText.new

      # set column information
      col = Gtk::TreeViewColumn.new("name", renderer, :text => COL_NAME)
      @treeView.append_column(col)

      col = Gtk::TreeViewColumn.new("namespace", renderer, :text => COL_NSPATH)
      @treeView.append_column(col)

      liststore.set_sort_column_id( COL_NAME )
    end

    def add celltype
      iter = @treeView.model.append
      iter[ COL_NAME ]  = celltype.get_name
      iter[ COL_NSPATH ]  = celltype.get_owner.get_namespace_path.to_s
    end

    def selected
      iter = @treeView.selection.selected
      if iter then
        [iter[ COL_NAME ], iter[ COL_NSPATH ] ]
      else
        [ nil, nil ]
      end
    end

    def delete item
    end

    def clear
      @treeView.model.clear
    end

    #=== CelltypeTreeView#get_treeView
    #RETURN::Gtk::TreeView
    def get_treeView
      @treeView
    end
  end # class CelltypeTreeView

  #== AttrTreeView: show cell list
  # formerly this class is sub-class of Gtk::TreeView
  # currently this class has Gtk::TreeView
  class AttrTreeView # < Gtk::TreeView
    #@choice_list::{name=>ListStore}
    #@cell::TmCell
    #@ct_attr_list::{ String(attr_name) => String(initializer) }
    #@view::MainView
    #@treeView::Gtk::TreeView

    COL_NAME   = 0
    COL_TYPE = 1
    COL_VALUE = 2

    #=== initialize
    def initialize tv
      @treeView = tv

      combo_list = Gtk::ListStore.new(String)
      iter = combo_list.append
      iter[0] = "a0"
=begin
      combo_list = Gtk::ListStore.new(String, String, String)
      iter = combo_list.append
      iter[0] = "a0"
      iter[1] = "b0"
      iter[2] = "c0"
      iter = combo_list.append
      iter[0] = "a1"
      iter[1] = "b1"
      iter[2] = "c1"

      combo_list2 = Gtk::ListStore.new(String, String, String)
      iter = combo_list2.append
      iter[0] = "A0"
      iter[1] = "B0"
      iter[2] = "C0"
      iter = combo_list2.append
      iter[0] = "A1"
      iter[1] = "B1"
      iter[2] = "C1"
=end

      # create data model
      liststore = Gtk::ListStore.new(String, String, String)

      # set data model to tree view(self)
      @treeView.set_model(liststore)
   
      # create renderer for text
      renderer = Gtk::CellRendererText.new

      #----- set column information -----#

      # ATTRIBUTE column
      col = Gtk::TreeViewColumn.new("attribute", renderer, :text => COL_NAME)
      col.set_cell_data_func(renderer) { |col, renderer, model, iter|
        if iter[ COL_VALUE ] == nil || iter[ COL_VALUE ] == ""
          renderer.foreground = "red"
        elsif @cell.is_editable?
          renderer.foreground = "black"
        else
          renderer.foreground = "blue"
        end
      }
      @treeView.append_column(col)

      # TYPE column
      col = Gtk::TreeViewColumn.new("type", renderer, :text => COL_TYPE)
      col.set_cell_data_func(renderer) { |col, renderer, model, iter|
        if @cell.is_editable?
          renderer.foreground = "black"
        else
          renderer.foreground = "blue"
        end
      }
      @treeView.append_column(col)

      # VALUE column
      renderer = Gtk::CellRendererCombo.new
      renderer.text_column = 0
      renderer.model = combo_list
      col = Gtk::TreeViewColumn.new("value", renderer, :text => COL_VALUE )
      col.set_cell_data_func(renderer) { |col, renderer, model, iter|
        # p "iter[0]=#{iter[0]}"
        if @cell.get_attr_list[iter[ COL_NAME ].to_sym] == nil
          renderer.foreground = "orange"
        elsif @cell.is_editable?
          renderer.foreground = "black"
        else
          renderer.foreground = "blue"
        end

        if @cell.is_editable?
          renderer.editable = true
        else
          renderer.editable = false
        end

        if @choice_list[ iter[0] ]
          renderer.model = @choice_list[ iter[0] ]
          renderer.has_entry = false
          renderer.text_column = 0
        else
          renderer.model = nil
          renderer.text_column = 0
          renderer.has_entry = true
        end

=begin
        # if iter[2] && iter[2] != ""
        if iter[1] == "ID"
          renderer.model = combo_list
          renderer.has_entry = false
          renderer.text_column = 0
        elsif iter[1] == "SIZE"
          renderer.model = combo_list2
          renderer.has_entry = false
          renderer.text_column = 1
        elsif iter[1] == "PRI"
          renderer.model = combo_list
          renderer.has_entry = false
          renderer.text_column = 2
        else
          renderer.model = nil
          renderer.text_column = 0
          renderer.has_entry = true
        end
=end
      }
      renderer.signal_connect('edited') { |w, path, new_text|
        # new_text can be wrong if 'text_column' is changed in each row
        # after selection is changed, before sending signal, many rows are redrawn

        # p "new_text='#{new_text}'"
        if (iter = @treeView.model.get_iter(path))
          if new_text == nil || new_text == ""
            if @ct_attr_list[ iter[ COL_NAME ] ]
              iter[ COL_VALUE ] = @ct_attr_list[ iter[ COL_NAME ] ]
            else
              iter[ COL_VALUE ] = new_text
            end
            if new_text == ""
              new_text = nil
            end
          else
            iter[ COL_VALUE ] = new_text
          end
          @cell.set_attr( iter[ COL_NAME ].to_sym, new_text )
          @cell.get_model.set_undo_point
          @view.paint_canvas
        end
      }
      @treeView.append_column(col)

    end

    #=== AttrTreeView#set_cell
    #cell::TmCell
    def set_cell cell
      clear
      @cell = cell
      @choice_list = {}
      @ct_attr_list = {}
      cell_attr_list = cell.get_attr_list

      ct = @cell.get_celltype
      if ct
        #----- register attributes and initializer to tree view model -----#
        ct.get_attribute_list.each{ |attr|
          iter = @treeView.model.append
          name = attr.get_name.to_s
          if attr.get_initializer
            @ct_attr_list[ name ] = attr.get_initializer.to_CDL_str
          end

          iter[ COL_NAME ] = name
          iter[ COL_TYPE ] = "#{attr.get_type.get_type_str}#{attr.get_type.get_type_str_post}"
          if cell_attr_list[ name.to_sym ]
            iter[ COL_VALUE ] = cell_attr_list[name.to_sym]
          elsif attr.get_initializer
            iter[ COL_VALUE ] = attr.get_initializer.to_CDL_str
          else
            # iter[ COL_VALUE ] = " "
          end

          #----- choice list model -----#
          if attr.get_choice_list
            @choice_list[ name ] = Gtk::ListStore.new(String)
            attr.get_choice_list.each{ |choice|
              iter = @choice_list[ name ].append
              iter[0] = CDLString.remove_dquote( choice.val )
            }
          end
        }
      end
    end

    def clear
      @treeView.model.clear
    end

    #=== AttrTreeView#set_view
    #view::MainView
    def set_view view
      @view = view
    end

    #=== AttrTreeView#get_treeView
    #RETURN::Gtk::TreeView
    def get_treeView
      @treeView
    end
  end # class AttrTreeView

  #== manage hilited objects
  class Hilite_objs
    #@hilite_objs::[TmCell|TmJoinBar]
    def initialize
      @hilite_objs = []
    end

    def add obj
      reset_if_ncessary obj
      @hilite_objs << obj
      @hilite_objs.uniq!
      update_attrTreeView
    end

    #=== hilite_objs#add_del
    # add if not include, delete if include
    def add_del obj
      reset_if_ncessary obj
      if @hilite_objs.include? obj
        @hilite_objs.delete obj
      else
        @hilite_objs << obj
      end
      update_attrTreeView
    end

    def reset obj = nil
      @hilite_objs = []
      if obj
        @hilite_objs << obj
      end
      update_attrTreeView
    end

    #=== hilite_objs#reset_if_ncessary
    # Port and ( Cell or Bar ) cannot be hilited simultaneously.
    # Ports belonging to diferent Cell cannot be hilited simultaneously.
    #obj::TmCell | TmBar | TmPort: new object to be hilited
    def reset_if_ncessary obj
      if @hilite_objs.length > 0
        if @hilite_objs[0].kind_of? TECSModel::TmPort
          if obj.kind_of? TECSModel::TmPort
            if obj.get_owner_cell != @hilite_objs[0].get_owner_cell
              reset
            end
          else
            reset
          end
        else
          if obj.kind_of? TECSModel::TmPort
            reset
          end
        end
      end
    end

    def each &proc #proc
      # Ruby3.0: obsolete
      # proc = Proc.new
      @hilite_objs.each{ |obj|
        proc.call obj
      }
    end

    def empty?
      @hilite_objs.empty?
    end

    def include? object
      @hilite_objs.include? object 
    end

    def set_attrTreeView treeview, name_entry, region_entry, frame
      @cell_property_frame = frame
      @cell_name_entry = name_entry
      @cell_region_entry = region_entry
      @attrTreeView = treeview
    end

    def change_cell_name name
     if @hilite_objs.length == 1 && @hilite_objs[0].kind_of?( TECSModel::TmCell )
       @hilite_objs[0].change_name name.to_sym
       @hilite_objs[0].get_model.set_undo_point
     end
    end

    def cell_plugin_dialog
     if @hilite_objs.length == 1 && @hilite_objs[0].kind_of?( TECSModel::TmCell )
       dialog = CellPluginDialog.new @hilite_objs[0]
       dialog.run
     end
    end

    def update_attrTreeView
      cell = nil
      n_cell = 0
      each{ |obj|
        if obj.kind_of? TECSModel::TmCell
          cell = obj
          n_cell += 1
        end
      }
      if n_cell == 1
        @cell_name_entry.text = cell.get_name.to_s
        @cell_region_entry.text = cell.get_region.get_namespace_path.to_s

        # this doesn't work!  I don't know how to change the color of Entry text
        if cell.is_editable?
          @cell_name_entry.modify_fg Gtk::STATE_NORMAL, Gdk::Color.parse( "black" )
          @cell_region_entry.modify_fg Gtk::STATE_NORMAL, Gdk::Color.parse( "black" )
          @cell_property_frame.set_label "cell property"
        else
          @cell_name_entry.modify_fg Gtk::STATE_NORMAL, Gdk::Color.parse( "blue" )
          @cell_region_entry.modify_fg Gtk::STATE_NORMAL, Gdk::Color.parse( "blue" )
          @cell_property_frame.set_label "cell property (read only)"
        end

        @cell_name_entry.set_editable cell.is_editable?
        @cell_region_entry.set_editable cell.is_editable?

        @attrTreeView.set_cell cell
      else
        @cell_name_entry.text = "(unselected)"
        @cell_name_entry.set_editable false
        @cell_name_entry.text = "(unselected)"
        @cell_name_entry.set_editable false
        @cell_property_frame.set_label "cell property (unselected)"

        @attrTreeView.clear
      end
    end

  end # class hilite_objs
end
