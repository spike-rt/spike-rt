Module_pre = "module TECSCDE\
  class CellPluginDialog\
    def get_ui_definition\
      return <<__EOT__"

all : cell-plugin-dialog-ui-definition.rb palette-ui-definition.rb

cell-plugin-dialog-ui-definition.rb : ../../glade/tecscde-cell-plugin.glade
	@echo updating $@
	@echo Module_pre             > $@
	@cat $< >> $@
	@echo "__EOT__"   >> $@
	@echo "    end"   >> $@
	@echo "  end"     >> $@
	@echo "end"       >> $@

palette-ui-definition.rb : ../../glade/tecscde-palette.glade
	@echo updating $@
	@echo "module TECSCDE"              > $@
	@echo "  class Palette"  >> $@
	@echo "    def get_ui_definition" >> $@
	@echo "      return <<__EOT__"  >> $@
	@cat $< >> $@
	@echo "__EOT__"   >> $@
	@echo "    end"   >> $@
	@echo "  end"     >> $@
	@echo "end"       >> $@

