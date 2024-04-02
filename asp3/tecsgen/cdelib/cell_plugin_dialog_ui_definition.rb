module TECSCDE
  class CellPluginDialog
    def get_ui_definition
      return <<__EOT__
<?xml version="1.0"?>
<interface>
  <requires lib="gtk+" version="2.16"/>
  <!-- interface-naming-policy project-wide -->
  <object class="GtkListStore" id="liststore_cell_plugin_list">
    <columns>
      <!-- column-name Cell -->
      <column type="gchararray"/>
      <!-- column-name Option -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">AAA</col>
        <col id="1" translatable="yes">BBB</col>
      </row>
    </data>
  </object>
  <object class="GtkDialog" id="dialog_cell_plugin">
    <property name="border_width">5</property>
    <property name="title" translatable="yes">Cell Plugin</property>
    <property name="modal">True</property>
    <property name="type_hint">normal</property>
    <property name="skip_taskbar_hint">True</property>
    <property name="skip_pager_hint">True</property>
    <property name="has_separator">False</property>
    <child internal-child="vbox">
      <object class="GtkVBox" id="dialog-vbox1">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">2</property>
        <child>
          <object class="GtkVBox" id="vbox1">
            <property name="visible">True</property>
            <property name="orientation">vertical</property>
            <child>
              <object class="GtkHBox" id="hbox1">
                <property name="visible">True</property>
                <child>
                  <object class="GtkLabel" id="label1">
                    <property name="visible">True</property>
                    <property name="xalign">0.89999997615814209</property>
                    <property name="xpad">10</property>
                    <property name="ypad">1</property>
                    <property name="label" translatable="yes">Cell Name</property>
                    <property name="justify">right</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="label_cell_name">
                    <property name="visible">True</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">CellName</property>
                    <property name="selectable">True</property>
                  </object>
                  <packing>
                    <property name="position">1</property>
                  </packing>
                </child>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkFrame" id="frame1">
                <property name="visible">True</property>
                <property name="label_xalign">0</property>
                <child>
                  <object class="GtkVBox" id="vbox2">
                    <property name="visible">True</property>
                    <property name="orientation">vertical</property>
                    <child>
                      <object class="GtkTreeView" id="treeview_plugin_list">
                        <property name="width_request">250</property>
                        <property name="height_request">80</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="model">liststore_cell_plugin_list</property>
                        <property name="search_column">0</property>
                      </object>
                      <packing>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkTable" id="table1">
                        <property name="visible">True</property>
                        <property name="n_rows">2</property>
                        <property name="n_columns">2</property>
                        <child>
                          <object class="GtkComboBox" id="combobox_cell_plugin">
                            <property name="visible">True</property>
                          </object>
                          <packing>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="y_options"></property>
                            <property name="x_padding">5</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_plugin_option">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="invisible_char">&#x25CF;</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_padding">5</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label7">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="xpad">5</property>
                            <property name="label" translatable="yes">Select Plugin</property>
                          </object>
                          <packing>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox4">
                            <property name="visible">True</property>
                            <child>
                              <object class="GtkLabel" id="label6">
                                <property name="visible">True</property>
                                <property name="xalign">0.10000000149011612</property>
                                <property name="label" translatable="yes">Plugin Option String</property>
                              </object>
                              <packing>
                                <property name="padding">5</property>
                                <property name="position">3</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkButton" id="button_add">
                                <property name="label" translatable="yes">Add</property>
                                <property name="width_request">60</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="padding">4</property>
                                <property name="pack_type">end</property>
                                <property name="position">2</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkButton" id="button_change">
                                <property name="label" translatable="yes">Change</property>
                                <property name="width_request">60</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="padding">1</property>
                                <property name="pack_type">end</property>
                                <property name="position">1</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkButton" id="button_del">
                                <property name="label" translatable="yes">Del</property>
                                <property name="width_request">60</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="padding">4</property>
                                <property name="pack_type">end</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="padding">5</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                </child>
                <child type="label">
                  <object class="GtkLabel" id="label3">
                    <property name="visible">True</property>
                    <property name="label" translatable="yes">&lt;b&gt;Plugin List&lt;/b&gt;</property>
                    <property name="use_markup">True</property>
                  </object>
                </child>
              </object>
              <packing>
                <property name="position">1</property>
              </packing>
            </child>
            <child>
              <object class="GtkFrame" id="frame2">
                <property name="visible">True</property>
                <property name="label_xalign">0</property>
                <child>
                  <object class="GtkTextView" id="textview_description">
                    <property name="width_request">250</property>
                    <property name="height_request">80</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="editable">False</property>
                  </object>
                </child>
                <child type="label">
                  <object class="GtkLabel" id="label2">
                    <property name="visible">True</property>
                    <property name="label" translatable="yes">&lt;b&gt;Plugin Descriiption&lt;/b&gt;</property>
                    <property name="use_markup">True</property>
                  </object>
                </child>
              </object>
              <packing>
                <property name="position">2</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="position">1</property>
          </packing>
        </child>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="dialog-action_area1">
            <property name="visible">True</property>
            <property name="layout_style">end</property>
            <child>
              <object class="GtkButton" id="button_ok">
                <property name="label" translatable="yes">OK</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkButton" id="button_cancel">
                <property name="label" translatable="yes">Cancel</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="pack_type">end</property>
            <property name="position">0</property>
          </packing>
        </child>
      </object>
    </child>
    <action-widgets>
      <action-widget response="0">button_ok</action-widget>
      <action-widget response="0">button_cancel</action-widget>
    </action-widgets>
  </object>
  <object class="GtkAction" id="action1"/>
</interface>
__EOT__
    end
  end
end
