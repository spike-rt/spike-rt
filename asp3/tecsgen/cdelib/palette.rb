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

 $Id: palette.rb 2640 2017-06-03 11:27:12Z okuma-top $

=end

module TECSCDE
  class Palette
    #control::TECSCDE::Control
    def initialize control
      @control = control
      Dir.chdir( "#{$tecsgen_base_path}/../glade" ){
        setup
      }
      @window.show_all
    end

    def setup
      @builder = Gtk::Builder.new
#      @builder.add get_ui_definition
      @builder.add_from_file "tecscde-palette.glade"

      #----- window -----#
      @window = @builder[ "window_palette" ]
      @window.realize
#      @window.type = ( Gtk::Window::TOPLEVEL )
      @window.window.set_functions( Gdk::Window::FUNC_RESIZE | Gdk::Window::FUNC_MOVE )

      #----- save BUTTON -----#
      @button_save = @builder[ "button_save" ]
      @button_save.signal_connect("clicked") { @control.on_save }

      #----- export BUTTON -----#
      @button_export = @builder[ "button_export" ]
      @button_export.signal_connect("clicked") {  @control.on_export   }

      #----- pointer BUTTON -----#
      @button_pointer = @builder[ "togglebutton_pointer" ]
      @button_pointer.signal_connect("clicked") {
        @control.on_pointer
        # @button_pointer.active = true
        @button_new_cell.active = false
        false
      }

      #----- new cell BUTTON -----#
      @button_new_cell = @builder[ "togglebutton_new_cell" ]
      @button_new_cell.signal_connect("clicked") {
        @control.on_new_cell
        # @button_new_cell.active = true
        @button_pointer.active = false
        false
      }

      #----- undo BUTTON -----#
      @button_undo = @builder[ "button_undo" ]
      @button_undo.signal_connect("clicked") {  @control.on_undo   }

      #----- redo BUTTON -----#
      @button_redo = @builder[ "button_redo" ]
      @button_redo.signal_connect("clicked") {  @control.on_redo   }
      # currently redo doesn't work well
      @button_redo.set_sensitive false

      #----- celltype TREEVIEW -----#
      @treeView_celltype = @builder[ "treeview_celltype" ]
      @celltypeTreeView = CelltypeTreeView.new @treeView_celltype

      #-----  -----#
      @frame_cell = @builder[ "frame_cell" ]

      #----- cell name ENTRY -----#
      @entry_cell_name = @builder[ "entry_cell_name" ]
      @entry_cell_name.signal_connect("activate"){|entry|
        @control.on_cell_name_entry_active entry
        false
      }
      @entry_cell_name.signal_connect("focus-out-event"){|entry,event|
        @control.on_cell_name_entry_focus_out entry
        false
      }

      @entry_cell_region = @builder[ "entry_cell_region" ]
      @entry_cell_region.signal_connect("activate"){|entry|
        @control.on_cell_region_entry_active entry
        false
      }
      @entry_cell_region.signal_connect("focus-out-event"){|entry,event|
        @control.on_cell_region_entry_focus_out entry
        false
      }

      @treeview_cell_attribute = @builder[ "treeview_cell_attribute" ]
      @attrTreeView = AttrTreeView.new @treeview_cell_attribute

      #----- quit BUTTON -----#
      @button_quit = @builder[ "button_quit" ]
      @button_quit.signal_connect("clicked") { @control.on_quit }

      @control.set_attrOperationWidgets @window, @celltypeTreeView, @attrTreeView, @entry_cell_name, @entry_cell_region, @frame_cell
    end

    def set_view view
      @window.set_transient_for( view.get_window )
      @window.window.set_group view.get_window.window
      @window.window.raise
    end
  end
end
