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

 $Id: tview.rb 2640 2017-06-03 11:27:12Z okuma-top $

=end

=begin

 (1) structure of MainWindow

   +- @mainWindow::Window---------------------+
   |+-@vbox::VBox(1/2)-----------------------+|
   ||+-- @scrolledWindow::ScrolledWindow---+ ||
   ||| +---------------------------------+ | ||
   ||| | @canvas::Canvas                 | | ||
   ||| |                                 | | ||
   ||| |                                 | | ||
   ||| |                                 | | ||
   ||| |                                 | | ||
   ||| |                                 | | ||
   ||| |                                 | | ||
   ||| |                                 | | ||
   ||| +---------------------------------+ | ||
   ||+-------------------------------------+ ||
   |+-@vbox::VBox(2/2)-----------------------+|
   ||                        <--HScale-->    ||
   |+----------------------------------------+|
   +------------------------------------------+

 @canvas::Canvas (<DrawingArea)
   紙の大きさを持つ、描画エリア
   大きさ (dots) = PaperHeight(mm) * dpi / 25.4 * Scale
     A4L=270*180  (Papersize=297*197)

 (2) canvasPixmap

   +---------------------------------+
   | @canvasPixmap::Pixmap           |
   |                                 |
   |                                 |
   |                                 |
   |                                 |
   |                                 |
   |                                 |
   |                                 |
   +---------------------------------+

 @canvasPixmap is invisible.
 draw contents on @canvasPixmap then copy on @canvas, to avoid flickers and to redraw fast on expose.

=end


module TECSCDE

  DPI = 96.0                                  # Dot per Inch
#  DPI = 72.0                                  # Dot per Inch

  ScaleHeight = 50                            # Height of HScale widget

  Scale = 1.0                                 # Scale initial value
  ScaleValIni = Scale * 100                   # 100%
  ScaleValMax = ScaleValIni * 2.00            # 200%
  ScaleValMin = ScaleValIni * 0.05            #   5%

  Triangle_Len     = 3                        # edge length(mm)
  Triangle_Height  = 2.598                    # height (mm)

  #----- draw text argment value -----#
  # object
  CELL_NAME         = 1
  CELLTYPE_NAME     = 2
  CELL_NAME_L       = 3
  SIGNATURE_NAME    = 4
  PORT_NAME         = 5
  PAPER_COMMENT     = 6

  # text alignment
  ALIGN_CENTER      = Pango::Layout::ALIGN_CENTER
  ALIGN_LEFT        = Pango::Layout::ALIGN_LEFT
  ALIGN_RIGHT       = Pango::Layout::ALIGN_RIGHT

  # text direction
  TEXT_HORIZONTAL   = 1   # left to right
  TEXT_VERTICAL     = 2   # bottom to top

  #----- Cursor for mouse pointer -----#
  CURSOR_PORT       = Gdk::Cursor.new Gdk::Cursor::SB_LEFT_ARROW
  CURSOR_JOINING    = Gdk::Cursor.new Gdk::Cursor::DOT
  CURSOR_JOIN_OK    = Gdk::Cursor.new Gdk::Cursor::CIRCLE
  CURSOR_NORMAL     = Gdk::Cursor.new Gdk::Cursor::TOP_LEFT_ARROW

  GapActive         = 1   # (mm)  gap of active cell between inner rectangle and outer one
  GapPort           = 0.8 # (mm)  gap between port name & edge

  #----- Paper -----#
  PAPER_MARGIN = 10 # (mm)

  #----- constnts for divString -----#
  Char_A = "A"[0]
  Char_Z = "Z"[0]
  Char__ = "_"[0]

  #----- Color -----#
  Color_editable_cell = :gray97
  Color_uneditable    = :blue
  Color_editable      = :black
  Color_hilite        = :magenta
  Color_incomplete    = :red
  Color_unjoin        = :magenta
    # color names are found in setup_colormap

  #=== Canvas class
  # This 'canvas' is for TECSCDE.  not relate to Gtk::Canvas.
  class Canvas < Gtk::DrawingArea
  end

  #== MainView class
  class MainView
    #@mainWindow::Gtk::Window
    #@mainWindowHeight::Integer
    #@mainWindowWidth::Integer
    #@vbox::VBox
    #@canvas_height::Integer
    #@canvas_width::Integer
    #@canvas::Canvas
    #@canvasPixmap::Gtk::Pixmap
    #@gdkWindow::Gdk::Window  GDK window of @canvas
    #@drawTarget::Gtk::Pixmap | Gdk::Window : @canvasPixmap or @gdkWindow
    #@canvasGc::Gdk::GC
    #@model::Model
    #@hScale::HScale
    #@scale_val::Integer
    #@control::Control
    #@pango_context::Gdk::Pango.context
    #@pango_layout::Pango::Layout
    #@pango_matrix::Pango::Matrix

    #colors
    @@colors = nil
    @@colormap = nil

    def initialize( model, control )
      @model = model
      @control = control
      @paper = :A3L
      @b_emphasize_cell_name = false
      @b_color_by_region = false
      MainView.setup_colormap

      @mainWindow = Gtk::Window::new(Gtk::Window::TOPLEVEL)
      @mainWindowWidth = width = 900
      @mainWindowHeight = height = 600
      @mainWindow.title = "TECSCDE - TECS Component Diagram Editor"
      @mainWindow.set_default_size(width, height)
      @mainWindow.sensitive = true
      @mainWindow.signal_connect( "delete-event" ){ |window, *args|
        dbgPrint "delete" + args.to_s + "\n"
        Gtk.main_quit
      }
      # KEY-PRESS event action
      @mainWindow.signal_connect("key-press-event"){|win,event|
        if @entryWin.visible?
          # while cell name editing, send forward to Entry window
          event.set_window @entryWin.window
          event.put
        else
          @control.key_pressed( event.keyval & 0xff, event.state )
        end
      }
      @mainWindow.signal_connect("focus-in-event"){|win,event|
        # p "event:#{event.class} in"
      }
      @mainWindow.signal_connect("focus-out-event"){|win,event|
        # p "event:#{event.class} out"
      }
      @mainWindow.signal_connect("grab-broken-event"){|win,event|
        # p "event:#{event.class}"
      }
      @mainWindow.signal_connect("grab-focus"){|win|
        # p "event:grab-focus"
      }
      @mainWindow.signal_connect("grab-notify"){|win, arg1|
        # p "event:grab-notify"
      }

      createHScale
      createHBox

      @vbox = Gtk::VBox.new
      # @vbox.set_resize_mode Gtk::RESIZE_IMMEDIATE
      #p @vbox.resize_mode
      @mainWindow.add @vbox

      @scrolledWindow = Gtk::ScrolledWindow.new
      # @scrolledWindow.signal_connect("expose_event") { |win, evt|
      #   gdkWin = @scrolledWindow.window
      #   gc = Gdk::GC.new gdkWin
      #   gdkWin.draw_rectangle( gc, true, 0, 0, 10000, 10000 )
      # }

      @vbox.pack_start @scrolledWindow
      @vbox.pack_end @hbox, false  # expand = false

      createCanvas
      @scrolledWindow.set_size_request( width, height - ScaleHeight )

      @mainWindow.show_all

      create_edit_window
    end

    def get_window
      @mainWindow
    end

    #------ CANVAS  ------#

    #=== create canvas
    def createCanvas

      @canvas = Canvas.new( )
      resize_canvas
      dbgPrint( "canvas width=#{@canvas_width}, height=#{@canvas_height}\n" )

      # BUTTON PRESS event action
      @canvas.signal_connect( "button-press-event" ){ |canvas, event|        # canvas = @canvas
        dbgPrint "pressed" + event.to_s + "\n"
        xd, yd = event.coords
        xm = dot2mm xd
        ym = dot2mm yd

        case event.event_type
        when Gdk::Event::BUTTON_PRESS   # single click or before ddouble, triple click
          click_count = 1
        when Gdk::Event::BUTTON2_PRESS  # double click
          click_count = 2
        when Gdk::Event::BUTTON3_PRESS  # triple click
          click_count = 3
        else
          click_count = 1
        end
        @control.pressed_on_canvas xm, ym, event.state, event.button, event.time, click_count
      }
      # BUTTON RELEASE event action
      @canvas.signal_connect( "button-release-event" ){ |canvas, event|
        dbgPrint "released" + event.to_s + "\n"
        xd, yd = event.coords
        xm = dot2mm xd
        ym = dot2mm yd
        @control.released_on_canvas xm, ym, event.state, event.button
      }
      # MOTION event action
      @canvas.signal_connect( "motion-notify-event" ){ |canvas, event|
        dbgPrint "motion" + event.to_s + "\n"
        xd, yd = event.coords
        xm = dot2mm xd
        ym = dot2mm yd
        @control.motion_on_canvas xm, ym, event.state
      }
      # EXPOSE event action
      @canvas.signal_connect("expose_event") { |win, evt|
        refresh_canvas
      }

      # add events to receive
      @canvas.add_events( Gdk::Event::POINTER_MOTION_MASK |
                          Gdk::Event::BUTTON_PRESS_MASK  |
                          Gdk::Event::BUTTON_RELEASE_MASK |
                          Gdk::Event::PROPERTY_CHANGE_MASK |
                          Gdk::Event::KEY_PRESS_MASK )

      @scrolledWindow.add_with_viewport @canvas
      # it seems that gdkWindow is nil before window.show or realize
      @canvas.realize
      @gdkWindow = @canvas.window
      @canvasGc = Gdk::GC.new @gdkWindow

      # prepare pixmap (buffer for canvas)
      #  pixmap cannot be resized, so we have the largest one at initial.
      @canvasPixmap = Gdk::Pixmap.new( @gdkWindow,
                                       @canvas_width  * ScaleValMax / ScaleValIni,
                                       @canvas_height * ScaleValMax / ScaleValIni,
                                       @gdkWindow.depth )
      # @drawTarget = @canvasPixmap
      @cairo_context_pixmap = @canvasPixmap.create_cairo_context
      @cairo_context_pixmap.save
      # @cairo_context_win = @gdkWindow.create_cairo_context
      # @cairo_context_win.save
      @cairo_context_target = @cairo_context_pixmap
      @cairo_matrix = MyCairoMatrix.new

      # prepare text renderer
      @pango_context = Gdk::Pango.context
      @pango_layout = Pango::Layout.new @pango_context
      @pango_matrix = Pango::Matrix.new.rotate!( 90 )
    end

    def paint_canvas
      clearCanvasPixmap

      #----- draw cells -----#
      @model.get_cell_list.each{  |cell|
        drawCell cell
      }

      #----- draw joins -----#
      # draw linew before draw texts (if other colors are used, it is better to lay texts upper side)
      @model.get_join_list.each{  |join|
        drawJoin join
      }

      refresh_canvas
    end

    def refresh_canvas
      @gdkWindow.draw_drawable( @canvasGc, @canvasPixmap, 0, 0, 0, 0, @canvas_width, @canvas_height )
      draw_hilite_objects @control.get_hilite_objs
    end

    def resize_canvas
      @canvas_height = Integer( mm2dot @model.get_paper[ :height ] )
      @canvas_width  = Integer( mm2dot @model.get_paper[ :width  ]  )
      @canvas.set_size_request( @canvas_width, @canvas_height )
      # @scrolledWindow.queue_draw
    end

    def clearCanvasPixmap

      @canvasGc.function = Gdk::GC::SET
      @canvasGc.fill = Gdk::GC::SOLID
      @canvasGc.foreground = Gdk::Color.new( 255, 255, 255 )
      @canvasPixmap.draw_rectangle( @canvasGc, true, 0, 0, @canvas_width, @canvas_height )
      canvasGC_reset
      # p "color = #{@canvasGc.foreground.red}, #{@canvasGc.foreground.green}, #{@canvasGc.foreground.blue}"
    end

    def set_cursor cursor
      @canvas.window.cursor = cursor 
    end

    #=== TmView#drawTargetDirect
    #change draw target to Window
    def drawTargetDirect
      # @drawTarget = @gdkWindow
      # @cairo_context_target = @cairo_context_win
    end

    #=== TmView#drawTargetReset
    #reset draw target to canvasPixmap
    def drawTargetReset
      # @drawTarget = @canvasPixmap
      # @cairo_context_target = @cairo_context_pixmap
    end

    #------ HBox  ------#
    def createHBox
      @hbox = Gtk::HBox.new
      #----- emphasize_cell_name button -----#
      @emphasize_cell_name_button = Gtk::ToggleButton.new( "Emphasize Cell Name" )
      @emphasize_cell_name_button.signal_connect("toggled") { |button|
        @b_emphasize_cell_name = button.active?
        paint_canvas
      }
      @hbox.pack_start @emphasize_cell_name_button

      #----- color by region button -----#
      #@color_by_region_button = Gtk::ToggleButton.new( "Color by Region" )
      @color_by_region_button = Gtk::CheckButton.new( "Color by Region" )
      @color_by_region_button.signal_connect("toggled") { |button|
        @b_color_by_region = button.active?
        # @color_by_region_button.label =  button.active? ? "Color by File" : "Color by Region"
        paint_canvas
      }
      @hbox.pack_start @color_by_region_button
      @hbox.pack_end @hScale
    end

    #------ HScale  ------#
    def createHScale
      @scale_val = ScaleValIni
      @hScale = Gtk::HScale.new( ScaleValMin, ScaleValMax, 1 )
      @hScale.set_digits 0   # 小数点以下
      @hScale.set_value @scale_val
      @hScale.set_size_request( @mainWindowWidth, ScaleHeight )
      @hScale.signal_connect( "value-changed" ){ |scale_self, scroll_type|
        # set scale_val in the range [ScaleValMin..ScaleValMax]
        scale_val = scale_self.value
        if scale_val > ScaleValMax
          scale_val = ScaleValMax
        elsif scale_val < ScaleValMin
          scale_val = ScaleValMin
        end
        @scale_val = scale_val
        dbgPrint "scale_val=#{@scale_val}\n"

        resize_canvas
        paint_canvas
      }
    end

    #------ Draw Contents on CANVAS  ------#

    def drawCell cell
      #----- calc position in dot -----#
      x, y, w, h = cell.get_geometry
      x1 = mm2dot x
      y1 = mm2dot y
      x2 = mm2dot( x + w )
      y2 = mm2dot( y + h )
      w1 = mm2dot( w )
      h1 = mm2dot( h )

      #----- paint cell -----#
      color = get_cell_paint_color cell
#      @canvasGc.set_foreground color
#      @drawTarget.draw_rectangle( @canvasGc, true, x1, y1, w1, h1 )

      @cairo_context_target.rectangle(x1, y1, w1, h1)
      @cairo_context_target.set_source_color( color )
      @cairo_context_target.fill

      #----- setup color -----#
      if ! cell.is_editable?
#        @canvasGc.set_foreground @@colors[ Color_uneditable ]
        @cairo_context_target.set_source_color @@colors[ Color_uneditable ]
      else
#        @canvasGc.set_foreground @@colors[ Color_editable ]
        @cairo_context_target.set_source_color @@colors[ Color_editable ]
      end

      #----- draw cell rect -----#
#      @drawTarget.draw_rectangle( @canvasGc, false, x1, y1, w1, h1 )
#      @cairo_context_target.rectangle(x1, y1, w1, h1)
      @cairo_context_target.rectangle(x1+0.5, y1+0.5, w1, h1)
      @cairo_context_target.set_line_width(1)
      @cairo_context_target.stroke

      gap = mm2dot GapActive
      gap = 2 if gap < 2  # if less than 2 dots, let gap 2 dots
      if cell.get_celltype && cell.get_celltype.is_active? then
#        @drawTarget.draw_rectangle( @canvasGc, false, x1 + gap, y1 + gap, w1 - 2 * gap, h1 - 2 * gap )
        @cairo_context_target.rectangle(x1 + gap + 0.5, y1 + gap + 0.5, w1 - 2 * gap, h1 - 2 * gap)
        @cairo_context_target.set_line_width(1)
        @cairo_context_target.stroke
      end

      #----- draw entry ports triangle -----#
      cell.get_eports.each{ |name, eport|
        if ! eport.is_array?
          draw_entry_port_triangle( eport )
        else
          if cell.is_editable? && eport.is_unsubscripted_array?
            # @canvasGc.set_foreground @@colors[ :brown ]
            @cairo_context_target.set_source_color @@colors[ :brown ]
          end
          # EPortArray
          eport.get_ports.each{ |ep|
            draw_entry_port_triangle( ep )
          }
          if cell.is_editable? && eport.is_unsubscripted_array?
            # @canvasGc.set_foreground @@colors[ Color_editable ]
            @cairo_context_target.set_source_color @@colors[ Color_editable ]
          end
        end
      }

      #----- draw cell name & celltype name -----#
      cell_name = cell.get_name
      ct_name = cell.get_celltype.get_name
      label = cell_name.to_s + "\n" + ct_name.to_s
      if ! cell.complete?
        # @canvasGc.set_foreground @@colors[ Color_incomplete ]
        @cairo_context_target.set_source_color @@colors[ Color_incomplete ]
      end
      # draw_text( x1 + w1/2, y1+h1/2, label, CELL_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )

      if @b_emphasize_cell_name
        wmn, hmn = get_text_extent( cell_name.to_s, CELL_NAME_L, ALIGN_CENTER, TEXT_HORIZONTAL )
        if wmn > w
          s1, s2 = divString cell_name.to_s
          draw_text( x1 + w1/2, y1+  h1/2 - mm2dot(hmn)/2, s1, CELL_NAME_L, ALIGN_CENTER, TEXT_HORIZONTAL )
          draw_text( x1 + w1/2, y1+  h1/2 + mm2dot(hmn)/2, s2, CELL_NAME_L, ALIGN_CENTER, TEXT_HORIZONTAL )
        else
          draw_text( x1 + w1/2, y1+h1/2, cell_name.to_s, CELL_NAME_L, ALIGN_CENTER, TEXT_HORIZONTAL )
        end
      else
        wmn, hmn = get_text_extent( cell_name.to_s, CELL_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
        draw_text( x1 + w1/2, y1+h1/2 + mm2dot(hmn)/2, cell_name.to_s, CELL_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
        draw_text( x1 + w1/2, y1+h1/2 - mm2dot(hmn)/2, ct_name.to_s,   CELLTYPE_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
      end

      #----- draw port name -----#
      (cell.get_cports.merge cell.get_eports).each{ |name,port|
        if ! port.is_array?
          set_port_color port, cell
          draw_port_name( port )
        else
          #--- prot array ---#
          port.get_ports.each{ |pt|
            set_port_color pt, cell
            draw_port_name( pt )
          }
        end
      }

      canvasGC_reset
    end

    #=== set_port_color
    def set_port_color port, cell
      if port.complete?
        if cell.is_editable?
          color_name = Color_editable
        else
          color_name = Color_uneditable
        end
      else
        if port.kind_of?( TECSModel::TmCPort ) && ! port.is_optional?
          color_name = Color_incomplete
        else
          color_name = Color_unjoin
        end
      end
      # @canvasGc.set_foreground @@colors[ color_name ]
      @cairo_context_target.set_source_color @@colors[ color_name ]
    end

    def draw_entry_port_triangle( eport )
      triangle_1_2 = mm2dot Triangle_Len / 2
      triangle_hi  = mm2dot Triangle_Height
      x1, y1 = eport.get_position
      xe = mm2dot x1
      ye = mm2dot y1
      case eport.get_edge_side
      when TECSModel::EDGE_TOP
        points = [[xe-triangle_1_2, ye], [xe+triangle_1_2, ye], [xe, ye+triangle_hi]]
      when TECSModel::EDGE_BOTTOM
        points = [[xe-triangle_1_2, ye], [xe+triangle_1_2, ye], [xe, ye-triangle_hi]]
      when TECSModel::EDGE_LEFT
        points = [[xe, ye-triangle_1_2], [xe, ye+triangle_1_2], [xe+triangle_hi,ye]]
      when TECSModel::EDGE_RIGHT
        points = [[xe, ye-triangle_1_2], [xe, ye+triangle_1_2], [xe-triangle_hi,ye]]
      end
#      fill = true
#      @drawTarget.draw_polygon( @canvasGc, fill, points )
      @cairo_context_target.triangle( *points[0], *points[1], *points[2] )
      @cairo_context_target.fill
    end

    def draw_port_name( port )
      x1, y1 = port.get_position
      xp = mm2dot x1
      yp = mm2dot y1
      case port.get_edge_side
      when TECSModel::EDGE_TOP
        alignment = ALIGN_LEFT
        direction = TEXT_VERTICAL
      when TECSModel::EDGE_BOTTOM
        alignment = ALIGN_RIGHT
        direction = TEXT_VERTICAL
      when TECSModel::EDGE_LEFT
        alignment = ALIGN_RIGHT
        direction = TEXT_HORIZONTAL
      when TECSModel::EDGE_RIGHT
        xp += 2
        alignment = ALIGN_LEFT
        direction = TEXT_HORIZONTAL
      end
      name = port.get_name.to_s
      if port.get_subscript != nil
        name += "[#{port.get_subscript}]"
      end
      name = port.get_name.to_s
      subscript = port.get_subscript
      if subscript
        if subscript >= 0
          name += "[#{subscript}]"
        end
      end
      draw_text( xp, yp, name, PORT_NAME, alignment, direction )
    end

    #=== TView#draw_hilite_objects
    def draw_hilite_objects obj_list
      obj_list.each{|obj|
        if obj.kind_of? TECSModel::TmCell
          drawCellRectDirect  obj
          # drawTargetDirect
          # drawCell obj
          # drawTargetReset
        elsif obj.kind_of? TECSModel::TmPort
          drawPortDirect  obj
        elsif obj.kind_of? TECSModel::TmJoinBar
          drawBarDirect obj
        end
      }
    end

    #=== TView#drawCellRectDirect
    # directly draw on Window hilited cell rect
    def drawCellRectDirect cell
      drawTargetDirect

      #----- set line width -----#
      canvasGC_set_line_width 2
      # @cairo_context_target.set_line_width(2)

      #----- if uneditable change color ------#
      if ! cell.is_editable?
        @canvasGc.set_foreground( @@colors[ Color_uneditable ] )
        # @cairo_context_target.set_source_color( @@colors[ Color_uneditable ] )
      end

      #----- calc position in dot -----#
      x, y, w, h = cell.get_geometry
      x1 = mm2dot x
      y1 = mm2dot y
      w1 = mm2dot( w )
      h1 = mm2dot( h )
      
      #----- draw cell rect -----#
      @gdkWindow.draw_rectangle( @canvasGc, false, x1, y1, w1, h1 )
      # @cairo_context_target.rectangle(x1, y1, w1, h1)
      # @cairo_context_target.stroke

      #----- reset GC, line width -----#
      canvasGC_reset
      canvasGC_set_line_width 1
      drawTargetReset
    end

    def drawPortDirect port
      drawTargetDirect

      #----- set line width -----#
      @canvasGc.set_foreground( @@colors[ Color_hilite ] )
      # @cairo_context_target.set_source_color( @@colors[ Color_hilite ] )
      draw_port_name port

      if port.kind_of? TECSModel::TmEPort
        draw_entry_port_triangle( port )
      end

      canvasGC_set_line_width 2
      x, y = port.get_position
      x1 = x2 = mm2dot x
      y1 = y2 = mm2dot y
      case port.get_edge_side
      when TECSModel::EDGE_TOP
        y1 -= 20
      when TECSModel::EDGE_BOTTOM
        y2 += 20
      when TECSModel::EDGE_LEFT
        x1 -= 20
      when TECSModel::EDGE_RIGHT
        x2 += 20
      end
      @gdkWindow.draw_line( @canvasGc, x1, y1, x2, y2 )
      # @cairo_context_target.move_to( x1, y1 )
      # @cairo_context_target.line_to( x2, y2 )

      #----- reset GC, line width -----#
      canvasGC_reset
      canvasGC_set_line_width 1

      drawTargetReset
    end

    def drawJoin join
      cport, eport, bars = join.get_ports_bars
      x, y = cport.get_position
      xm = mm2dot( x ) + 0.5
      ym = mm2dot( y ) + 0.5

      #----- setup color -----#
      if ! join.is_editable?
        # @canvasGc.set_foreground @@colors[ Color_uneditable ]
        @cairo_context_target.set_source_color( @@colors[ Color_uneditable ] )
      end

      @cairo_context_target.move_to xm, ym
      #----- draw bars -----#
      bars.each{ |bar|
        if bar.instance_of? TECSModel::HBar then
          xm2 = mm2dot( bar.get_position ) + 0.5
          # @drawTarget.draw_line( @canvasGc, xm, ym, xm2, ym )
          @cairo_context_target.line_to xm2, ym
          xm = xm2
        else  # VBar
          ym2 = mm2dot( bar.get_position ) + 0.5
          # @drawTarget.draw_line( @canvasGc, xm, ym, xm, ym2 )
          @cairo_context_target.line_to xm, ym2
          ym = ym2
        end
      }
      @cairo_context_target.set_line_width(1)
      @cairo_context_target.stroke

      #----- draw signature name -----#
      if eport.get_joins[0] == join
        # draw only 1st entry port join

        if ( eport.get_subscript == nil || eport.get_subscript == 0 ) &&
            ( join.get_cport.get_subscript == nil || join.get_cport.get_subscript == 0 )

          if bars[2].instance_of? TECSModel::VBar
            xm = mm2dot( (bars[1].get_position + bars[3].get_position)/2 )
            ym = mm2dot( bars[2].get_position + 2 )
          else
            xm = mm2dot( (bars[0].get_position + bars[2].get_position)/2 )
            ym = mm2dot( bars[1].get_position + 2 )
          end
          draw_text( xm, ym, join.get_signature.get_name.to_s, SIGNATURE_NAME, ALIGN_CENTER, TEXT_HORIZONTAL )
        end
      end

      canvasGC_reset
    end

    #=== TView#drawBarDirect
    # directly draw on Window
    def drawBarDirect bar
      drawTargetDirect

      join = bar.get_join
      cport, eport, bars = join.get_ports_bars
      x, y = cport.get_position
      xm = mm2dot x
      ym = mm2dot y

      canvasGC_set_line_width 2

      bars.each{ |bar2|
        if @control.get_hilite_objs.include? bar2
          color = @@colors[Color_hilite]
        elsif join.is_editable?
          color = @@colors[Color_editable]
        else
          color = @@colors[Color_uneditable]
        end
        @canvasGc.foreground = color
        @cairo_context_target.set_source_color color

        if bar2.instance_of? TECSModel::HBar then
          xm2 = mm2dot( bar2.get_position )
          @gdkWindow.draw_line( @canvasGc, xm, ym, xm2, ym )
          xm = xm2
        else  # VBar
          ym2 = mm2dot( bar2.get_position )
          @gdkWindow.draw_line( @canvasGc, xm, ym, xm, ym2 )
          ym = ym2
        end
      }

      canvasGC_set_line_width 1
      canvasGC_reset

      drawTargetReset
    end

    #----- draw and utility for text  -----#

    def get_text_extent( text, obj_type, alignment, direction )
      if direction != TEXT_VERTICAL then
        pc = @pango_context
        plo = @pango_layout
        pc.matrix = nil
        plo.text = text.to_s
        pfd = pc.font_description
        pfd.absolute_size = font_size( obj_type )
        plo.font_description = pfd
        plo.alignment = alignment
        # plo.context_changed          # ??
        rect2 = plo.get_pixel_extents[1]
        return [ dot2mm(rect2.rbearing), dot2mm(rect2.descent) ]
      else
        pc = @pango_context
        plo = @pango_layout
        pm = @pango_matrix
        pc.matrix = pm
        plo.text = text.to_s
        pfd = pc.font_description
        pfd.absolute_size = font_size( obj_type )
        plo.font_description = pfd
        plo.alignment = alignment
        # plo.context_changed
        rect2 = plo.get_pixel_extents[1]
        return [ dot2mm(rect2.descent), dot2mm(rect2.rbearing) ]
      end
    end

    #x::Integer(dot)
    #y::Integer(dot)
    #obj_type::CELL_NAME, SIGNATURE_NAME, PORT_NAME
    #alignment::ALIGN_CENTER, ALIGN_LEFT
    def draw_text( x, y, text, obj_type, alignment, direction )
      if direction == TEXT_VERTICAL then
        draw_text_v( x, y, text, obj_type, alignment )
      else
        draw_text_h( x, y, text, obj_type, alignment )
      end
    end

    def draw_text_h( x, y, text, obj_type, alignment )
      # draw_text_h_gdk( x, y, text, obj_type, alignment )
      draw_text_h_cairo( x, y, text, obj_type, alignment )
      # draw_text_h_cairo_pango( x, y, text, obj_type, alignment )
    end

    def draw_text_h_gdk( x, y, text, obj_type, alignment )
      #----- Gdk Pango version -----#
      pc = @pango_context
      plo = @pango_layout
      pc.matrix = nil
      plo.text = text
      pfd = pc.font_description
      pfd.absolute_size = font_size( obj_type )
      plo.font_description = pfd
      plo.alignment = alignment
      # plo.context_changed          # ??
      rect2 = plo.get_pixel_extents[1]

      case alignment
      when ALIGN_CENTER
        # calc text draww postion
        x2 = x - rect2.rbearing / 2
        y2 = y - rect2.descent / 2
      when ALIGN_RIGHT
        x2 = x - rect2.rbearing - mm2dot( GapPort )
        y2 = y - rect2.descent
      when ALIGN_LEFT
        x2 = x + mm2dot( GapPort )
        y2 = y - rect2.descent
      end

      # pfd =  Pango::FontDescription.new
      # p pfd.size, pfd.variant, pfd.family
      # rect = plo.get_pixel_extents[0]
      # p rect.ascent, rect.descent, rect.lbearing, rect.rbearing
      # p rect2.ascent, rect2.descent, rect2.lbearing, rect2.rbearing

      @drawTarget.draw_layout( @canvasGc, x2, y2, plo )
    end

      #----- Cairo version -----#
    def draw_text_h_cairo( x, y, text, obj_type, alignment )
      cr = @cairo_context_target
      cr.select_font_face( font_family = nil, # "courier", # font_family = "Times New Roman",
                           font_slant  = Cairo::FONT_SLANT_NORMAL,
                           font_weight = Cairo::FONT_WEIGHT_NORMAL )
      cr.set_font_size( font_size( obj_type ) / 1000 )
      cr_te = cr.text_extents(text)
      # p "width=#{cr_te.width} x_bearing=#{cr_te.x_bearing} height=#{cr_te.height} y_bearing=#{cr_te.y_bearing}"
      case alignment
      when ALIGN_CENTER
        # calc text draww postion
        x2 = x - ( cr_te.width  + cr_te.x_bearing ) / 2
        y2 = y - ( cr_te.y_bearing ) / 2
      when ALIGN_RIGHT
        x2 = x - cr_te.width - cr_te.x_bearing - mm2dot( GapPort )
        y2 = y - cr_te.height - cr_te.y_bearing - 2
      when ALIGN_LEFT
        x2 = x + mm2dot( GapPort )
        y2 = y - cr_te.height - cr_te.y_bearing - 2
      end
      cr.move_to(x2, y2)
      cr.show_text(text)
    end

      #----- Cairo Pango version -----#
    def draw_text_h_cairo_pango( x, y, text, obj_type, alignment )
      cr = @cairo_context_target
      # pfd = Pango::FontDescription.new( "Times" )
      pfd = Pango::FontDescription.new
      pfd.absolute_size = font_size( obj_type )
      plo = cr.create_pango_layout
      plo.font_description = pfd
      plo.alignment = alignment
      plo.set_text text
      rect2 = plo.get_pixel_extents[1]

      case alignment
      when ALIGN_CENTER
        # calc text draww postion
        x2 = x - rect2.rbearing / 2
        y2 = y - rect2.descent / 2
      when ALIGN_RIGHT
        x2 = x - rect2.rbearing - mm2dot( GapPort )
        y2 = y - rect2.descent
      when ALIGN_LEFT
        x2 = x + mm2dot( GapPort )
        y2 = y - rect2.descent
      end
      cr.move_to(x2, y2)
      cr.show_pango_layout( plo )
    end

    #x::Integer(dot)
    #y::Integer(dot)
    #obj_type::CELL_NAME, SIGNATURE_NAME, PORT_NAME
    #alignment::ALIGN_CENTER, ALIGN_LEFT
    def draw_text_v( x, y, text, obj_type, alignment )
      # draw_text_v_gdk( x, y, text, obj_type, alignment )
      draw_text_v_cairo( x, y, text, obj_type, alignment )
      # draw_text_v_cairo_pango( x, y, text, obj_type, alignment )
    end
      #----- Gdk Pango version -----#
    def draw_text_v_gdk( x, y, text, obj_type, alignment )
      pc = @pango_context
      plo = @pango_layout
      pm = @pango_matrix
      pc.matrix = pm
      plo.text = text
      pfd = pc.font_description
      pfd.absolute_size = font_size( obj_type )
      plo.font_description = pfd
      plo.alignment = alignment
      # plo.context_changed
      rect2 = plo.get_pixel_extents[1]

      case alignment
      when ALIGN_CENTER
        # calc text draww postion
        x2 = x - rect2.descent / 2
        y2 = y - rect2.rbearing / 2
      when ALIGN_RIGHT
        x2 = x - rect2.descent
        y2 = y + mm2dot( GapPort )
      when ALIGN_LEFT
        x2 = x - rect2.descent
        y2 = y - rect2.rbearing - mm2dot( GapPort )
      end

      @drawTarget.draw_layout( @canvasGc, x2, y2, plo )
    end

    #----- Cairo version -----#
    def draw_text_v_cairo( x, y, text, obj_type, alignment )
      cr = @cairo_context_target
      cr.select_font_face( font_family = nil, # "courier", # font_family = "Times New Roman",
                           font_slant  = Cairo::FONT_SLANT_NORMAL,
                           font_weight = Cairo::FONT_WEIGHT_NORMAL )
      cr.set_font_size( font_size( obj_type ) / 1000 )
      cr_te = cr.text_extents(text)
      # p "width=#{cr_te.width} x_bearing=#{cr_te.x_bearing} height=#{cr_te.height} y_bearing=#{cr_te.y_bearing}"
      case alignment
      when ALIGN_CENTER   # this case is not used & not checked
        # calc text draww postion
        x2 = x - 2
        y2 = y - ( cr_te.width  + cr_te.x_bearing ) / 2
      when ALIGN_RIGHT
        x2 = x - 2
        y2 = y + cr_te.width  + cr_te.x_bearing + mm2dot( GapPort )
      when ALIGN_LEFT
        x2 = x - 2
        y2 = y - mm2dot( GapPort )
      end
      @cairo_matrix.set_rotate90( x2, y2 )  # rotate around (0, 0) then shift (x2, y2)
      cr.matrix = @cairo_matrix
      cr.move_to(0, 0)  # this assumes that (0, 0) is left bottom of strings
      cr.show_text(text)
      @cairo_matrix.set_rotate0
      cr.matrix = @cairo_matrix
    end

      #----- Cairo Pango version -----#
    def draw_text_v_cairo_pango( x, y, text, obj_type, alignment )
      cr = @cairo_context_target
      # pfd = Pango::FontDescription.new( "Times" )
      pfd = Pango::FontDescription.new
      pfd.absolute_size = font_size( obj_type )
      # p "font_size=#{font_size( obj_type )}"
      plo = cr.create_pango_layout
      plo.font_description = pfd
      plo.alignment = alignment
      plo.set_text text
      rect2 = plo.get_pixel_extents[1]
      # p "descent=#{rect2.descent}, rbearing=#{rect2.rbearing}"

      case alignment
      when ALIGN_CENTER
        # calc text draww postion
        x2 = x - rect2.descent / 2
        y2 = y - rect2.rbearing / 2
      when ALIGN_RIGHT
        x2 = x
        y2 = y + rect2.rbearing + mm2dot( GapPort )
      when ALIGN_LEFT
        x2 = x
        y2 = y - mm2dot( GapPort )
      end

      matrix = Cairo::Matrix.new( 0, -1, 1, 0, x2, y2 )
      cr.matrix = matrix
      cr.move_to(0, 0)  # this assumes that (0, 0) is left bottom of strings
      cr.show_text(text)
      cr.matrix = Cairo::Matrix.new( 1, 0, 0, 1, 0, 0 )
    end

    #---------- Cell name editor ---------#
    def create_edit_window
      @entry = Gtk::Entry.new
      @entry.set_has_frame true

      @entryWin = Gtk::Window.new Gtk::Window::TOPLEVEL
      @entryWin.add @entry
      @entryWin.realize
      @entryWin.window.reparent @canvas.window, 0, 0  # Gdk level operation

      # these steps are to avoid to move ( 0, 0 ) at 1st appear
      @entryWin.show_all
      @entryWin.hide
    end

    def begin_edit_name cell, time
      @entry.set_text cell.get_name

      x, y, w, h = get_cell_name_edit_area cell
      # p "x=#{x} y=#{y} w=#{w} h=#{h}"
      @entryWin.window.move( x - 3, y - 6 )    # Gdk level operation
      @entryWin.window.resize( w + 6, h + 8 )  # Gdk level operation
      @entryWin.show_all
    end

    def end_edit_name
      name = @entry.text
      @entryWin.hide
      return name
    end

    def get_cell_name_edit_area cell
      name = cell.get_name
      obj_type = CELL_NAME
      alignment = ALIGN_CENTER
      direction = TEXT_HORIZONTAL
      wmn, hmn = get_text_extent( name, obj_type, alignment, direction )
      xm, ym, wm, hm = cell.get_geometry
      x = mm2dot( xm + ( wm - wmn ) / 2 )
      y = mm2dot( ym + hm / 2 + 1 )
#      y = mm2dot( ym + hm / 2 - hmn )
      w = mm2dot wmn
      h = mm2dot hmn

      return [ x, y, w, h ]
    end

    #------ Convert Unit  ------#

    #=== convert mm to dot
    def mm2dot mm
      ( @scale_val * mm * DPI / 25.4 / 100 ).to_i
    end

    #=== convert dot to mm
    def dot2mm dot
      dot * 100 * 25.4 / DPI / @scale_val
    end

    #=== font_size
    # obj_type::Integer CELL_NAME, SIGNATURE_NAME, PORT_NAME
    def font_size obj_type
      case obj_type
      when CELL_NAME
        base_size = 10500
      when CELLTYPE_NAME
        base_size = 10500
      when CELL_NAME_L
        base_size = 16000
      when SIGNATURE_NAME
        base_size = 9000
      when PORT_NAME
        base_size = 9000
      when PAPER_COMMENT
        base_size = 10500
      end
      base_size * @scale_val / 100.0 * DPI / 96.0
    end

    #------ handle CanvasGC  ------#
    def canvasGC_reset
      @canvasGc.function = Gdk::GC::COPY
      @canvasGc.fill = Gdk::GC::SOLID
      @canvasGc.foreground = @@colors[Color_editable]

      @cairo_context_target.restore
      @cairo_context_target.save    # prepare for next time
      @cairo_context_target.matrix = @cairo_matrix
    end

    def canvasGC_set_line_width width
      line_attr = @canvasGc.line_attributes
      line_width = line_attr[0]
      line_attr[0] = width
      @canvasGc.set_line_attributes *line_attr
    end

    def self.setup_colormap
      if @@colors != nil
        return
      end

      @@colors = {}
      @@colormap = Gdk::Colormap.system

      [ :black, :white, :gray, :yellow, :orange, :skyblue, :magenta, :red, :blue, :green,
        :cyan, :brown, :violet, :lavender, :MistyRose, :lightyellow, :LightCyan, :Beige,
        :PapayaWhip, :Violet, :pink ].each{ |color_name|
        setup_colormap_1 color_name
      }
      setup_colormap_2 :ultraLightGreen, Gdk::Color.new( 0xE000, 0xFF00, 0xE000 )
      setup_colormap_1 Color_editable_cell

      @@cell_paint_colors = [ :MistyRose, :lightyellow, :LightCyan, :ultraLightGreen, :lavender, :Beige,
                              :PapayaWhip, :Violet, :pink ]
      # plum: light purble (pastel)
      # pink: light magenta (pastel)
      # lavender: light blue (pastel)
      # lightyellow: light yellow (pastel)
      @@cell_paint_color_index = 0
      @@cell_file_to_color = {}

    end

    def self.setup_colormap_1 name
      color = Gdk::Color.parse( name.to_s )        
      self.setup_colormap_2 name, color
    end

    def self.setup_colormap_2 name, color
      @@colors[ name ] = color
      @@colormap.alloc_color( color, false, true )
    end

    #----- cell paint colors -----#

    def get_cell_paint_color cell
      if @b_color_by_region
        region = cell.get_region
        color = @@cell_file_to_color[ region ]
        if color
          return color
        end
        obj = region
      else
        tecsgen_cell= cell.get_tecsgen_cell
        if tecsgen_cell == nil || cell.is_editable?
          return @@colors[ Color_editable_cell ]
        end
        file = tecsgen_cell.get_locale[0]
        color = @@cell_file_to_color[ file ]
        if color
          return color
        end
        obj = file
      end
      if @@cell_paint_color_index >= @@cell_paint_colors.length
        @@cell_paint_color_index = 0
      end
      col_name = @@cell_paint_colors[ @@cell_paint_color_index ]
      @@cell_file_to_color[ obj ] = @@colors[ col_name ]
      @@cell_paint_color_index += 1
      # p "col_name:#{col_name} index:#{@@cell_paint_color_index}"
      return @@colors[ col_name ]
    end

    #------ export ------#
    def export fname
      begin
        if File.exist?(fname)
          File.unlink fname
        end
      rescue => evar
        TECSCDE.message_box( "fail to remove #{fname}\n#{evar.to_s}", :OK )
        return
      end

      scale_val_bak = @scale_val
      @scale_val = 72.0 / TECSCDE::DPI * 100     # PDF surface = 72 DPI,  mm2dot assume 100 DPI by default
      target_bak = @cairo_context_target

      paper = Cairo::Paper.const_get( @model.get_paper[ :name ] )
      paper_width = paper.width( "pt" ) - mm2dot( PAPER_MARGIN * 2 )
      paper_height = paper.height( "pt" ) - mm2dot( PAPER_MARGIN * 2 ) 
      begin
        surface = Cairo::PDFSurface.new( fname, paper.width( "pt" ), paper.height( "pt" ) )
        @cairo_context_target = Cairo::Context.new surface

        #----- set paper margin -----#
        @cairo_matrix.set_base_shift( mm2dot( PAPER_MARGIN ), mm2dot( PAPER_MARGIN ) )
        @cairo_context_target.matrix = @cairo_matrix

        #----- clip in rectangle frame -----#
        @cairo_context_target.rectangle( 0, 0, paper_width, paper_height )
        @cairo_context_target.clip false # preserve = false
        @cairo_context_target.save      # (* pair *)   # must be saved initially

        #----- draw contents of PDF -----#
        paint_canvas

        #----- draw model name -----#
        draw_text( paper_width, paper_height, @model.get_file_editing, PAPER_COMMENT, ALIGN_RIGHT, TEXT_HORIZONTAL )
 
        #----- draw rectangle frame around paper -----#
        @cairo_context_target.rectangle( 0, 0, paper_width, paper_height)
        @cairo_context_target.stroke

        #----- complete PDF file -----#
        surface.finish

        #----- reset context -----#
        # cairo_context_target: unnecessary because the context is abandoned after this
        # @cairo_matrix.set_base_shift( 0, 0 )
        # @cairo_context_target.matrix = @cairo_matrix
        # @cairo_context_target.restore   # (* pair *)
      rescue => evar
        p evar
        TECSCDE.message_box( "fail to writ to #{fname}\n#{evar.to_s}", :OK )
      ensure
        @cairo_context_target = target_bak
        @cairo_matrix.set_base_shift( 0, 0 )
        @scale_val = scale_val_bak
        if surface
          surface.finish
        end
      end

      paint_canvas
    end

    #=== MainView#divString
    # divide string near center at A-Z or '_'
    def divString( str )
      len = str.length
      if len <= 4
        return [ str, "" ]
      end

      center = len / 2
      i = 0
      n = 0
      while( (center / 2 > i) && (i < center) && (str[ center + i ] != nil) )
        char_i = str[ center - i ]
        char_j = str[ center + i ]
        if char_j == Char__ || (Char_A <= char_j && char_j <= Char_Z)
          n = center + i
          break
        elsif (Char_A <= char_i && char_i <= Char_Z)
          n = center - i
          break
        elsif char_i == Char__
          n = center - i + 1
          break
        end
        i += 1
      end
      if n > 0
        return [ str[ 0, n ], str[ n, len ] ]
      else
        return [ str[ 0, len / 2 ], str[ len / 2, len ] ]
      end
    end

  end # class MainView

  #== MyCairoMatrix
  # this class is necessary for draw_text_v_cairo & totally shift when writing PDF
  class MyCairoMatrix < Cairo::Matrix
    def initialize
      @base_x = 0
      @base_y = 0
      super( 1, 0, 0, 1, 0, 0 )
    end

    def set(xx, yx, xy, yy, x0, y0)
      x0 += @base_x
      y0 += @base_y
      super
    end

    #=== MyCairoMatrix#set_rotate0
    # no rotate, then shift (x, y)
    def set_rotate0( x = 0, y = 0 )
      set(1, 0, 0, 1, x, y )
      self
    end

    #=== MyCairoMatrix#set_rotate90
    # rotate 90 around (0, 0) then shift (x, y)
    def set_rotate90( x, y )
      set(0, -1, 1, 0, x, y )
      self
    end

    def set_base_shift( x, y )
      @base_x = x
      @base_y = y
      set_rotate0
    end
                  
  end
end
