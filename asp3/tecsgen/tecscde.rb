#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

=begin

TECSCDE - TECS Component Diagram Editor

Copyright (C) 2014-2019 by TOPPERS Project

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

 $Id: tecscde.rb 2640 2017-06-03 11:27:12Z okuma-top $

=end

#= TECSCDE - TECS Component Diagram Editor
#Authors::   Hiroshi OYAMA
#Copyright:: TOPPERS Project, 2014. All rights reserved.
#License::   TOPPERS License (written above and each file)

# tecsgen.rb, tecscde.rb, tecslib & cdelib must be in the same directory
$tecsgen_base_path = File.dirname( File.expand_path __FILE__ )

require "gtk2"
require "date"
require "#{$tecsgen_base_path}/cdelib/version"
require "#{$tecsgen_base_path}/cdelib/tmodel"
require "#{$tecsgen_base_path}/cdelib/tview"
require "#{$tecsgen_base_path}/cdelib/tcontrol"
require "#{$tecsgen_base_path}/cdelib/saveload"
require "#{$tecsgen_base_path}/cdelib/cell_plugin_dialog"
require "#{$tecsgen_base_path}/cdelib/palette.rb"

$tool_version = $tecscde_version

def dbgPrint( message, *args )
  printf( message, *args )
  print "\n"
  STDOUT.flush
end

def flush_print( message, *args )
  print( message, *args )
  STDOUT.flush
end

module TECSCDE

  #=== TECSCDE.message_box
  # Function to open a dialog box displaying the message provided.
  #ok_yn_okcan::
  #RETURN
  def self.message_box(message, ok_yn_okcan )
    # p "message_box #{message}"

    # Create the dialog
    dialog = Gtk::Dialog.new("Message",
                             nil,
                             nil,
                             [ Gtk::Stock::OK, Gtk::Dialog::RESPONSE_OK ])

    # Ensure that the dialog box is destroyed when the user responds.
    # dialog.signal_connect('response') { dialog.destroy }

    # Add the message in a label, and show everything we've added to the dialog.
    dialog.vbox.add(Gtk::Label.new(message))
    dialog.show_all

    res = nil
    dialog.run do |response|
      res = response
    end
    dialog.destroy
    res
  end

  def self.main( tecsgen )
    begin
      aSet = MainViewAndModel.new tecsgen
      # aSet.test_main
      Gtk::main()
      # rescue Exception => evar
      #  p "exception caught"
      # ensure
    end
  end

  def self.test
    begin
      aSet = MainViewAndModel.new
      aSet.test_main
      # rescue Exception => evar
      #  p "exception caught"
      # ensure
    end
  end

  class MainViewAndModel
    #@model::TECSModel
    #@view::MainView
    def initialize tecsgen
      @model   = TECSModel.new tecsgen
      @control = Control.new @model
      @view    = MainView.new @model, @control
      @control.set_view @view
      @model.set_view @view

      @model.add_cell_list_from_tecsgen
      @model.set_undo_point

      @view.paint_canvas
    end
    def get_model
      @model
    end

    def test_main
      p "test_main"
      cell1 = @model.new_cell( 0, 0 )
      cell2 = @model.new_cell( 100, 100 )

      cport = TECSModel::TmCPort.new cell1
      eport = TECSModel::TmEPort.new cell2

      join = @model.new_join( cport, eport )
      # p join
      # Gtk::main()
    end
  end

end

#--------------- begining of TECSCDE run -----------------#

$TECSCDE = true
$Title = "tecscde"
require "#{$tecsgen_base_path}/tecsgen.rb"

begin

  $tecscde = true    #Bool: true if tecscde
  $b_tate = true     #Bool: true if vertical style
  $b_force_apply_tool_info = false   #Bool: force to apply tool_info

  additional_option_parser = Proc.new { |parser|
    parser.on( '--base_dir=dir', 'base directory (tecscde only)' ){ |dir|
      $base_dir[ dir ] = true
    }
    parser.on( '--force-apply-tool_info', 'force to apply tool_info, even if validation failed. this might cause ruby exception and stop tecscde' ){
      $b_force_apply_tool_info = true
    }
    parser.on( '--tate', 'vertical (tate) style (tecscde only)' ){
      $b_tate = true
    }
    parser.on( '--yoko', 'horizontal (yoko) style (tecscde only)' ){
      $b_tate = false
    }
  }

  TECSGEN.init( additional_option_parser )
  tecsgen = TECSGEN.new
  tecsgen.run1
  TECSCDE.main tecsgen
#rescue => evar
#  STDERR << "tecsgen: exit because of unrecoverble error.\n"
#  STDERR << "   please retry after resolve early error.\n"
#  STDERR << "   if no error occured, please report to TOPPERS TECS WG (users@toppers.jp or com-wg@toppers.jp).\n"
#  exit 1
end


# TECSCDE.test

  
