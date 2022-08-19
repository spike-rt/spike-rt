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

 $Id: saveload.rb 2780 2018-02-11 11:22:33Z okuma-top $

=end


module TECSCDE
  class TECSModel

    # tool_info schema for tecscde
    @@TECSCDE_schema = {
      :tecscde => {
        :cell_list    => [ :cell_location ],      # array
        :join_list    => [ :join_location ]       # array
      },
      :__tecscde => {
        :paper        => :paper                   # paper
      },
      :paper  => {
        :type         => "paper",                 # fixed string (type name)
        :size         => :string,                 # "A4", "A3", "A2"
        :orientation  => :string,                 # "LANDSCAPE", "PORTRAIT"
      },
      :cell_location => {
        :type         => "cell_location",         # fixed string (type name)
        :region       => :string,                 # "rRegion::rReg"
        :name         => :string,                 # "CellName"
        :location     => [ :number ],             # [ x, y, w, h ]
        :port_location => [ :port_location ]      # array
      },
      :port_location => {
        :type         => "port_location",         # fixed string (type name)
        :port_name    => :string,
        :edge         => :string,                 # "EDGE_TOP" | "EDGE_BOTTOM" | "EDGE_LEFT" | "EDGE_RIGHT"
        :offset       =>  :number                 # real number (mm) (>=0)
      },
      :__port_location => {                       # port_location optional
        :subscript    => :integer
      },
      :join_location => {
        :type         => "join_location",         # fixed string (type name)
        :call_region  => :string,                 #"rRegionParent::rRegionChild",
        :call_cell    => :string,                 # "CellName",
        :call_port    => :string,                 # "cPort",
        :entry_region => :string,                 # "rERegionParent::rERegionChild",
        :entry_cell   => :string,                 # "ECellName",
        :entry_port   => :string,                 # "ePort",
        :bar_list     => [:HBar, :VBar]           # mixed (HBar&VBar) array type
      },
      :__join_location => {                       # join_location optional
        :call_port_subscript   => :integer,       # >= 0
        :entry_port_subscript  => :integer,       # >= 0
      },
      :HBar => {
        :type         => "HBar",                  # fixed string (type name)
        :position     => :number,                 # real number (mm), location in X-axis
      },
      :VBar => {
        :type         => "VBar",                  # fixed string (type name)
        :position     => :number,                 # real number (mm), location in Y-axis
      }
    }

    #=== TECSModel#add_cell_list_from_tecsgen
    def add_cell_list_from_tecsgen

      #----- set @file_editing -----#
      argv = TECSGEN.get_argv
      if argv.length > 0
        last_arg = argv[ -1 ]
        if ( last_arg =~ /\.cde\Z/ )
          @file_editing = last_arg
        else
          if last_arg =~ /\.cdl\Z/
            @file_editing = last_arg.gsub( /\.cdl\Z/, ".cde" )
          else
            @file_editing = last_arg + ".cde"
          end
        end
      else
        @file_editing = ""
      end

      print "file_editing: #{@file_editing}\n"

      tecsgen_cell_list = @tecsgen.get_cell_list
      tecsgen_cell_list2 = []
      x = 10
      y = 10
=begin
         x = @paper[ :width ] - 60
         y = @paper[ :height ] -30
=end

      cell_list = { }  # ::Cell => TmCell
      if tecsgen_cell_list then
        p "=== create cell ==="
        tecsgen_cell_list.each{ |cell|
          # p cell.get_owner.get_namespace
          # p cell.get_owner.get_namespace_path
          if @cell_hash[ cell.get_name ] then    # duplicate cell in cdl file
            next
          end
          if cell.get_celltype == nil then       # celltype not found error in cdl (tecsgen)
            p "add_cell: celltype not found: #{cell.get_name} #{cell.get_owner.get_namespace_path}"
            next
          end

          p "add_cell #{cell.get_name} #{cell.get_owner.get_namespace_path} #{cell.get_locale}"
          new_cell_ = create_cell_from_tecsgen( cell, x, y )
          tecsgen_cell_list2 << cell
          cell_list[ cell ] = new_cell_

          new_cell_.set_editable( cell.get_locale )

          x += 55
          if x >= @paper[ :width ] - 30
            x = 10
            y += 30
            if y >= @paper[ :height ] -15
              y = 10
            end
          end
=begin
          x -= 55
          if x <= 10
            x =   @paper[ :width ] - 60
            y -= 30
            if y <= 50
              y = @paper[ :height ] -30
            end
          end
=end
        }

        #------ validate and set location info from __tool_info( "tecscde" ) ------#
        # begin
        if validate || $b_force_apply_tool_info
          p "=== set_paper ==="
          set_paper_from_tecsgen

          p "=== set_cell_location ==="
          set_cell_location_from_tecsgen
        else
          TECSCDE.error( "validate error in __tool_info__( \"tecscde\" )")
        end

        p "=== create join ==="
        tecsgen_cell_list2.each{ |cell|
          cell.get_join_list.get_items.each{ |join|
            if join.get_array_member2 == nil
              create_join_from_tecsgen( cell, join, cell_list )
            else
              join.get_array_member2.each{ |j|
                if j != nil
                  create_join_from_tecsgen( cell, j, cell_list )
                end
              }
            end
          }
        }

        if validate || $b_force_apply_tool_info
          p "=== set_join_location ==="
          set_join_location_from_tecsgen
        end
      end
    end

    #=== TECSModel#create_cell_from_tecsgen
    def create_cell_from_tecsgen( cell, x, y )
      new_cell_ = new_cell( x, y, cell.get_celltype.get_name, cell.get_celltype.get_owner.get_namespace_path.to_s, cell )
      new_name = cell.get_name          # automatically given name
      new_cell_.change_name( new_name )
      
      # decide cell box size from text width
      w, h = @view.get_text_extent( new_name, CELL_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
      w2, h = @view.get_text_extent( cell.get_celltype.get_name, CELLTYPE_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
      w = w2 if w2 > w
      w += 2
      h += 2
      w = 25 if w < 25
      h = 15 if h < 15
      new_cell_.set_geometry( x, y, w, h )
      return new_cell_
    end

    #=== TECSModel#create_join_from_tecsgen
    def create_join_from_tecsgen( cell, join, cell_list )
      # p join.get_name
      object = cell.get_celltype.find join.get_name
      # p "OBJECT CLASS #{object.class}"
      if object.instance_of?( ::Port ) then
        if object.get_port_type == :CALL then
          if ! object.is_require? then
            lhs_cell = cell_list[ cell ]
            cport = lhs_cell.get_cport_for_new_join( join.get_name, join.get_subscript )
            if cport == nil
              TECSCDE.error( "#{@name}.#{join.get_name} not found")
              return
            end
            rhs_cell = cell_list[ join.get_cell ]
            if rhs_cell == nil then      # not joined in cdl (tecsgen)
              return
            end
            # eport = rhs_cell.get_eports[ join.get_port_name ]
            eport = rhs_cell.get_eport_for_new_join( join.get_port_name, join.get_rhs_subscript1 )
            # p "new_join #{lhs_cell.get_name}.#{cport.get_name} => #{rhs_cell.get_name}.#{eport.get_name}"
            new_join_ = new_join cport, eport
            new_join_.set_editable( join.get_locale )
          end
        end
      else
        cell_list[cell].set_attr( join.get_name, join.get_rhs.to_CDL_str )
      end
    end


    def set_paper_from_tecsgen
      info = TOOL_INFO.get_tool_info( :tecscde )
      if info == nil || info[ :paper ] == nil
        return
      end

      #----- paper -----#
      paper_info = info[ :paper ]
      if paper_info
        size = paper_info[ :size ]
        orientation = paper_info[ :orientation ]
        paper = nil
        Paper.each{ |name, val|
          if val[ :size ] == size && val[ :orientation ] == orientation
            p "paper found #{val[:name]}"
            paper = val
          end
        }
        @paper = paper if paper
      end
    end

    def set_cell_location_from_tecsgen
      info = TOOL_INFO.get_tool_info( :tecscde )
      if info == nil || info[ :cell_list ] == nil
        return
      end

      #----- cell location -----#
      info[ :cell_list ].each{ |cell_location|
        # region = cell_location[ :region ].to_sym
        name = cell_location[ :name ].to_sym
        loc = cell_location[ :location ]
        if loc.length != 4
          TECSCDE.error( "#{name}: cell_location.location: array length is not inconsistent, #{loc.length} for 4")
          next
        end
        cell = @cell_hash[ name ]
        if cell then
          # p "apply location: #{cell.get_name}"
          cell.set_geometry( *loc )

          #------ port location -----#
          cell_location[ :port_location ].each{ |port_location|
            # mikan offset not set yet
            port_name = port_location[ :port_name ].to_sym
            edge = get_edge_side_val( port_location[ :edge ] )
            offset = port_location[ :offset ]
            subscript = port_location[ :subscript ]
            port = cell.get_cports[ port_name ]
            if port == nil
              port = cell.get_eports[ port_name ]
            end
            if port == nil
              p "port '#{port_name}' not found"
              next
            end
            if subscript
              if ! port.is_array?
                p "port '#{port_name}' : 'subscript' specified but not array"
                next
              end
              if subscript <0
                p "port '#{port_name}' : 'subscript' negative valude specified"
                next
              end
              p0 = port
              port = port.get_ports[ subscript ]   # array
              if port == nil
                p "port '#{port_name}' : 'subscript=#{subscript}' out of range"
                next
              end
            else
              if port.is_array?
                p "port '#{port_name}' : array but no 'subscript' specified"
                next
              end
            end
            port.set_position edge, offset
          }
        else
          @cell_hash.each{|a,b| p a }
          p "not apply location: #{name}"
          next
        end
      }
    end

    def set_join_location_from_tecsgen
      info = TOOL_INFO.get_tool_info( :tecscde )
      if info == nil
        return
      end

      #----- join location -----#
      info[ :join_list ].each{ |jl|
        # jl[ :call_region ]
        cp_cell_nspath = jl[ :call_cell ].to_sym
        cp_name = jl[ :call_port ].to_sym
        cp_subscript = jl[ :call_port_subscript ]
        # jl[ :entry_region ]
        ep_cell_nspath = jl[ :entry_cell ].to_sym
        ep_name = jl[ :entry_port ].to_sym
        ep_subscript = jl[ :entry_port_subscript ]

        bl = jl[ :bar_list ]
        bar_list = []
        bl.each{ |bar|
          bar_list << [ bar[ :type ], bar[ :position ] ]
        }
        
        # cp_cell_nspath, cp_name, ep_cell_nspath, ep_name, bar_list = jl.get_location
        # p "set_location_from_tecsgen, #{cp_cell_nspath}, #{cp_name}, #{ep_cell_nspath}, #{ep_name}, #{bar_list}"
        cp_cell = @cell_hash[ cp_cell_nspath ]
        ep_cell = @cell_hash[ ep_cell_nspath ]
        # check existance of cells
        if cp_cell != nil && ep_cell != nil then
          cport = cp_cell.get_cports[ cp_name ]
          if cport.kind_of? TmCPortArray
            if cp_subscript == nil
              TECSCDE.error( "TM9999 location information ignored #{cp_name} is array but not specified subscript")
              next
            end
            cport = cport.get_member cp_subscript
          else
            if cp_subscript != nil
              TECSCDE.error( "TM9999 #{cp_name} is not array but specified subscript")
            end
          end
          eport = ep_cell.get_eports[ ep_name ]
          if eport.kind_of? TmEPortArray
            if ep_subscript == nil
              TECSCDE.error( "TM9999 location information ignored #{ep_name} is array but not specified subscript")
              next
            end
            eport = eport.get_member ep_subscript
          else
            if ep_subscript != nil
              TECSCDE.error( "TM9999 #{ep_name} is not array but specified subscript")
            end
          end
          # p "1 #{cp_name} #{cp_subscript} #{ep_name} #{ep_subscript} #{cport} #{eport}"

          # check existance of cport & eport and direction of bar & edge (must be in right angle)
          # mikan necessary more than 2 bars
          if cport != nil && eport != nil && eport.include?( cport.get_join( cp_subscript ) ) && bar_list.length >= 2 then
            # p "2"
            b_vertical = TECSModel.is_vertical?( cport.get_edge_side )
            bar_type = bar_list[0][0].to_sym
            if ( b_vertical  && bar_type == :HBar ) || (! b_vertical && bar_type == :VBar ) then
              # p "3"
              len = bar_list.length

              normal_pos = bar_list[len-1][1]
              tan_pos = bar_list[len-2][1]
              # p "normal_pos=#{normal_pos}, eport_normal=#{eport.get_position_in_normal_dir}"
              # p "tan_pos=#{tan_pos}, eport_tan=#{eport.get_position_in_tangential_dir}"
              # check if normal_pos & tan_pos can be evaluated and the position of bars goal
              if normal_pos != nil && tan_pos != nil &&
                  ( (normal_pos - eport.get_position_in_normal_dir).abs <= MAX_ERROR_IN_NOR ) &&
                  ( (tan_pos - eport.get_position_in_tangential_dir).abs <= MAX_ERROR_IN_TAN ) then
                # p "4"
                bars = []
                bar_list.each{ |bar_info|
                  # bar_list: array of [ IDENTIFER, position ] => bars ( array of HBar or VBar )
                  pos = bar_info[1]
                  if pos != nil && bar_info[0].to_sym == :HBar then
                    bar = HBar.new( pos, cport.get_join )
                    bars << bar
                  elsif pos != nil && bar_info[0].to_sym == :VBar then
                    bar = VBar.new( pos, cport.get_join )
                    bars << bar
                  else
                    bars = []
                    break
                  end
                }
                # mikan length more than 2
                len = bars.length
                if len >= 0.1 then
                  bars[ len - 1 ].set_position eport.get_position_in_normal_dir
                  bars[ len - 2 ].set_position eport.get_position_in_tangential_dir
                  # p "bar changed for #{cp_cell_nspath}.#{cport.get_name}"
                  cport.get_join.change_bars bars
                end
              end
            end
          end
        end
      }

    end

    #=== TECSModel#validate
    # validate JSON format data in __tool_info__( "tecscde" )
    def validate
      validator = TOOL_INFO::VALIDATOR.new( :tecscde, @@TECSCDE_schema )
      return validator.validate
    end

    #=== save data ***
    def save fname
      begin
        Dir.chdir $run_dir
        f = open( fname, "w" )
        save_tecsgen f
        save_cells f
        save_info f
        f.close
      rescue => evar
        TECSCDE.message_box( "fail to save #{fname}\n#{evar.to_s}", :OK )
      end
    end

    #=== TECSModel#save_tecsgen
    #output __tool_info__ ("tecsgen")
    def save_tecsgen f
      f.print "__tool_info__ (\"tecsgen\") {\n"

      f.print "    \"tecscde_version\"     : \"#{$tecscde_version}\",\n"
      f.print "    \"cde_format_version\"  : \"#{$cde_format_version}\",\n"
      f.print "    \"save_date\"           : \"#{DateTime.now}\",\n"

      f.print "    \"base_dir\" : [\n"
      delim = "        "
      $base_dir.each{ |bd, b_necessity|
        if b_necessity
          f.print "#{delim}\"#{bd}\""
          delim = ",\n        "
        end
      }
      f.print "\n    ],\n"

      f.print "    \"define_macro\" : [\n"
      delim = ""
      $define.each { |define|
        f.print "#{delim}        \"#{define}\""
        delim = ",\n"
      }
      f.print "\n    ],\n"

      f.print "   \"import_path\" : [\n"
      delim = ""
      $import_path.each{ |path|
        tecspath = $tecspath.gsub( /\\/, '\\\\\\\\' )
        # p tecspath
        # p path, Regexp.new( "^A#{tecspath}" ), path =~ Regexp.new( "\\A#{tecspath}" )
        next if path =~ Regexp.new( "\\A#{tecspath}" )
        f.print "#{delim}        \"#{path}\""
        delim = ",\n"
      }
      f.print "\n    ],\n"

      f.print "    \"direct_import\" : [\n"
      delim = ""
      Import.get_list.each { |path, import|
        if ( import.is_imported? == false ) && ( import.get_cdl_name != @file_editing )
          f.print "#{delim}        \"#{import.get_cdl_name}\""
          delim = ",\n"
        end
      }
      f.print "\n    ]"

      # optionaly set
      if $b_cpp_specified
        f.print ",\n    \"cpp\"    : \"#{$cpp}\"\n"
      else
        f.print "\n"
      end

      f.print "}\n\n"
    end

    #=== TECSModel#save_cells
    #output cell definition
    def save_cells f
      @cell_list.each{ |cell|                           # mikan region
        if ! cell.is_editable?
          next
        end

        f.print( "cell #{ cell.get_celltype.get_namespace_path} #{cell.get_name} {\n" )

        if cell.get_cports.length > 0
          f.print "\n    /*** call ports ***/\n"
        end
        cell.get_cports.each{ |name, cport|
          if cport.is_array?
            cport.get_ports.each{ |cport|
              join = cport.get_join
              if join then
                eport = join.get_eport
                if eport.get_subscript then
                  subscript = "[ #{eport.get_subscript} ]"
                else
                  subscript = ""
                end
                f.print "    #{cport.get_name}[ #{cport.get_subscript} ] = #{eport.get_cell.get_name}.#{eport.get_name}#{subscript};\n"
              end
            }
          else
            join = cport.get_join
            if join then
              eport = join.get_eport
              if eport.get_subscript then
                subscript = "[ #{eport.get_subscript} ]"
              else
                subscript = ""
              end
              f.print "    #{cport.get_name} = #{eport.get_cell.get_name}.#{eport.get_name}#{subscript};\n"
            end
          end
        }

        attr_list = cell.get_attr_list
        if attr_list.length > 0
          f.print "\n    /*** attributes ***/\n"
        end
        attr_list.keys.sort.each{ |attr|
          f.print "    #{attr} = #{attr_list[attr]};\n"
        }

        f.print( "};\n" )
      }
    end

    #=== TECSModel#save_info
    #output location information
    def save_info f
      f.print "\n"
      f.print "/*************************************************\n"
      f.print " *         TOOL INFORMATION FOR TECSCDE          *\n"
      f.print " *     This  information is used by tecscde      *\n"
      f.print " *  please don't touch if you are not familiar   *\n"
      f.print " ************************************************/\n"
      f.print "__tool_info__ (\"tecscde\") {\n"

      save_paper_info f
      save_cell_info f
      save_join_info f

      f.print "}\n"
    end

    #=== TECSModel#save_cell_info
    #output cell location information
    def save_paper_info f
      f.print <<EOT
    "paper" : {
       "type" : "paper",
       "size" :  "#{@paper[ :size ]}",
       "orientation" :  "#{@paper[ :orientation ]}"
    },
EOT
    end

    #=== TECSModel#save_cell_info
    #output cell location information
    def save_cell_info f
      f.print "    \"cell_list\" : [\n"
      delim_1 = ""
      index = 0
      @cell_list.each{ |cell|
        x, y, w, h = cell.get_geometry
        f.print <<EOT
#{delim_1}        {       /** cell_list[ #{index} ] **/
            "type"     : "cell_location",
            "name"     : "#{cell.get_name}",
            "location" : [ #{x}, #{y}, #{w}, #{h} ],
            "region"   : "#{cell.get_region.get_namespace_path.to_s}",
            "port_location" : [
EOT
        delim_2 = ""
        (cell.get_cports.merge cell.get_eports).each{ |name, cport|
          if cport.is_array?
            cport.get_ports.each{ |cp|
              f.print <<EOT
#{delim_2}                {
                    "type"      : "port_location",
                    "port_name" : "#{cp.get_name}",
                    "subscript" : #{cp.get_subscript},
                    "edge"      : "#{cp.get_edge_side_name}",
                    "offset"    : #{cp.get_offset}
EOT
              f.print "                }"
              delim_2 = ",\n"
            }
          else
            f.print <<EOT
#{delim_2}                {
                    "type"      : "port_location",
                    "port_name" : "#{cport.get_name}",
                    "edge"      : "#{cport.get_edge_side_name}",
                    "offset"    : #{cport.get_offset}
EOT
            f.print "                }"
          end
          delim_2 = ",\n"
        }
        f.print "\n            ]\n        }"
        index += 1
        delim_1 = ",\n"
      }
      f.print "\n    ],\n"
    end

    #=== TECSModel#save_join_info
    #output join location information
    def save_join_info f
      f.print "    \"join_list\" : [\n"
      delim_1 = ""
      index = 0
      @join_list.each{ |join|
        cport, eport, bars = join.get_ports_bars
        if cport.get_subscript then
          cp_subsc = "            \"call_port_subscript\" : #{cport.get_subscript},\n"
        else
          cp_subsc = ""
        end
        if eport.get_subscript then
          ep_subsc = "            \"entry_port_subscript\" : #{eport.get_subscript},\n"
        else
          ep_subsc = ""
        end
# :call_region : 
# :entry_region : 

        f.print <<EOT
#{delim_1}        {       /** join_list[ #{index} ] **/
            "type"        : "join_location",
            "call_cell"   : "#{cport.get_cell.get_name}",
            "call_region" : "#{cport.get_cell.get_region.get_namespace_path.to_s}",
            "call_port"   : "#{cport.get_name}",
#{cp_subsc}            "entry_cell"  : "#{eport.get_cell.get_name}",
            "entry_region": "#{eport.get_cell.get_region.get_namespace_path.to_s}",
            "entry_port"  : "#{eport.get_name}",
#{ep_subsc}            "bar_list"    : [
EOT
        delim_2 = ""
        bars.each{ |bar|
          f.print <<EOT
#{delim_2}                {
                     "type"     : "#{(bar.instance_of? HBar) ? "HBar" : "VBar"}",
                     "position" : #{bar.get_position}
EOT
          f.print "                }"
          delim_2 = ","
        }
        f.print <<EOT

            ]
EOT
        f.print "        }"
        delim_1 = ",\n"
        index += 1
      }
      f.print "\n    ]\n"
    end

    #=== TECSModel#get_edge_side_val
    def get_edge_side_val edge_side_name
      case edge_side_name
      when "EDGE_TOP"
        EDGE_TOP
      when "EDGE_BOTTOM"
        EDGE_BOTTOM
      when "EDGE_LEFT"
        EDGE_LEFT
      when "EDGE_RIGHT"
        EDGE_RIGHT
      else
        0   # same as EDGE_TOP
      end
    end

  end #  class TECSModel
end # module TECSCDE

