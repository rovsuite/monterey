#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T20:24:49
#
#-------------------------------------------------

QT       += core gui network svg

TARGET = Monterey
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../QActivityMonitor/QActivityMonitor/qactivitymonitor.cpp \
    ../QBoolMonitor/QBoolMonitor/qboolmonitor.cpp \
    ../QROVMotor/QROVMotor/qrovmotor.cpp \
    ../QROVRelay/QROVRelay/qrovrelay.cpp \
    ../QROVServo/QROVServo/qrovservo.cpp \
    ../Qwtsrc/qwt_wheel.cpp \
    ../Qwtsrc/qwt_thermo.cpp \
    ../Qwtsrc/qwt_text.cpp \
    ../Qwtsrc/qwt_text_label.cpp \
    ../Qwtsrc/qwt_text_engine.cpp \
    ../Qwtsrc/qwt_system_clock.cpp \
    ../Qwtsrc/qwt_symbol.cpp \
    ../Qwtsrc/qwt_spline.cpp \
    ../Qwtsrc/qwt_slider.cpp \
    ../Qwtsrc/qwt_series_data.cpp \
    ../Qwtsrc/qwt_scale_widget.cpp \
    ../Qwtsrc/qwt_scale_map.cpp \
    ../Qwtsrc/qwt_scale_engine.cpp \
    ../Qwtsrc/qwt_scale_draw.cpp \
    ../Qwtsrc/qwt_scale_div.cpp \
    ../Qwtsrc/qwt_sampling_thread.cpp \
    ../Qwtsrc/qwt_round_scale_draw.cpp \
    ../Qwtsrc/qwt_raster_data.cpp \
    ../Qwtsrc/qwt_point_polar.cpp \
    ../Qwtsrc/qwt_point_3d.cpp \
    ../Qwtsrc/qwt_plot.cpp \
    ../Qwtsrc/qwt_plot_zoomer.cpp \
    ../Qwtsrc/qwt_plot_xml.cpp \
    ../Qwtsrc/qwt_plot_svgitem.cpp \
    ../Qwtsrc/qwt_plot_spectrogram.cpp \
    ../Qwtsrc/qwt_plot_spectrocurve.cpp \
    ../Qwtsrc/qwt_plot_seriesitem.cpp \
    ../Qwtsrc/qwt_plot_scaleitem.cpp \
    ../Qwtsrc/qwt_plot_rescaler.cpp \
    ../Qwtsrc/qwt_plot_renderer.cpp \
    ../Qwtsrc/qwt_plot_rasteritem.cpp \
    ../Qwtsrc/qwt_plot_picker.cpp \
    ../Qwtsrc/qwt_plot_panner.cpp \
    ../Qwtsrc/qwt_plot_marker.cpp \
    ../Qwtsrc/qwt_plot_magnifier.cpp \
    ../Qwtsrc/qwt_plot_layout.cpp \
    ../Qwtsrc/qwt_plot_item.cpp \
    ../Qwtsrc/qwt_plot_intervalcurve.cpp \
    ../Qwtsrc/qwt_plot_histogram.cpp \
    ../Qwtsrc/qwt_plot_grid.cpp \
    ../Qwtsrc/qwt_plot_directpainter.cpp \
    ../Qwtsrc/qwt_plot_dict.cpp \
    ../Qwtsrc/qwt_plot_curve.cpp \
    ../Qwtsrc/qwt_plot_canvas.cpp \
    ../Qwtsrc/qwt_plot_axis.cpp \
    ../Qwtsrc/qwt_picker.cpp \
    ../Qwtsrc/qwt_picker_machine.cpp \
    ../Qwtsrc/qwt_panner.cpp \
    ../Qwtsrc/qwt_painter.cpp \
    ../Qwtsrc/qwt_null_paintdevice.cpp \
    ../Qwtsrc/qwt_matrix_raster_data.cpp \
    ../Qwtsrc/qwt_math.cpp \
    ../Qwtsrc/qwt_magnifier.cpp \
    ../Qwtsrc/qwt_legend.cpp \
    ../Qwtsrc/qwt_legend_item.cpp \
    ../Qwtsrc/qwt_knob.cpp \
    ../Qwtsrc/qwt_interval.cpp \
    ../Qwtsrc/qwt_interval_symbol.cpp \
    ../Qwtsrc/qwt_event_pattern.cpp \
    ../Qwtsrc/qwt_dyngrid_layout.cpp \
    ../Qwtsrc/qwt_double_range.cpp \
    ../Qwtsrc/qwt_dial.cpp \
    ../Qwtsrc/qwt_dial_needle.cpp \
    ../Qwtsrc/qwt_curve_fitter.cpp \
    ../Qwtsrc/qwt_counter.cpp \
    ../Qwtsrc/qwt_compass.cpp \
    ../Qwtsrc/qwt_compass_rose.cpp \
    ../Qwtsrc/qwt_column_symbol.cpp \
    ../Qwtsrc/qwt_color_map.cpp \
    ../Qwtsrc/qwt_clipper.cpp \
    ../Qwtsrc/qwt_arrow_button.cpp \
    ../Qwtsrc/qwt_analog_clock.cpp \
    ../Qwtsrc/qwt_abstract_slider.cpp \
    ../Qwtsrc/qwt_abstract_scale.cpp \
    ../Qwtsrc/qwt_abstract_scale_draw.cpp \
    rovdebug.cpp \
    rovsettings.cpp \
    rovmappings.cpp \
    rovabout.cpp

HEADERS  += mainwindow.h \
    ../QActivityMonitor/QActivityMonitor/qactivitymonitor.h \
    ../QBoolMonitor/QBoolMonitor/qboolmonitor.h \
    ../QROVMotor/QROVMotor/qrovmotor.h \
    ../QROVRelay/QROVRelay/qrovrelay.h \
    ../QROVServo/QROVServo/qrovservo.h \
    ../Qwtsrc/qwt.h \
    ../Qwtsrc/qwt_wheel.h \
    ../Qwtsrc/qwt_thermo.h \
    ../Qwtsrc/qwt_text.h \
    ../Qwtsrc/qwt_text_label.h \
    ../Qwtsrc/qwt_text_engine.h \
    ../Qwtsrc/qwt_system_clock.h \
    ../Qwtsrc/qwt_symbol.h \
    ../Qwtsrc/qwt_spline.h \
    ../Qwtsrc/qwt_slider.h \
    ../Qwtsrc/qwt_series_data.h \
    ../Qwtsrc/qwt_scale_widget.h \
    ../Qwtsrc/qwt_scale_map.h \
    ../Qwtsrc/qwt_scale_engine.h \
    ../Qwtsrc/qwt_scale_draw.h \
    ../Qwtsrc/qwt_scale_div.h \
    ../Qwtsrc/qwt_sampling_thread.h \
    ../Qwtsrc/qwt_round_scale_draw.h \
    ../Qwtsrc/qwt_raster_data.h \
    ../Qwtsrc/qwt_point_polar.h \
    ../Qwtsrc/qwt_point_3d.h \
    ../Qwtsrc/qwt_plot.h \
    ../Qwtsrc/qwt_plot_zoomer.h \
    ../Qwtsrc/qwt_plot_svgitem.h \
    ../Qwtsrc/qwt_plot_spectrogram.h \
    ../Qwtsrc/qwt_plot_spectrocurve.h \
    ../Qwtsrc/qwt_plot_seriesitem.h \
    ../Qwtsrc/qwt_plot_scaleitem.h \
    ../Qwtsrc/qwt_plot_rescaler.h \
    ../Qwtsrc/qwt_plot_renderer.h \
    ../Qwtsrc/qwt_plot_rasteritem.h \
    ../Qwtsrc/qwt_plot_picker.h \
    ../Qwtsrc/qwt_plot_panner.h \
    ../Qwtsrc/qwt_plot_marker.h \
    ../Qwtsrc/qwt_plot_magnifier.h \
    ../Qwtsrc/qwt_plot_layout.h \
    ../Qwtsrc/qwt_plot_item.h \
    ../Qwtsrc/qwt_plot_intervalcurve.h \
    ../Qwtsrc/qwt_plot_histogram.h \
    ../Qwtsrc/qwt_plot_grid.h \
    ../Qwtsrc/qwt_plot_directpainter.h \
    ../Qwtsrc/qwt_plot_dict.h \
    ../Qwtsrc/qwt_plot_curve.h \
    ../Qwtsrc/qwt_plot_canvas.h \
    ../Qwtsrc/qwt_picker.h \
    ../Qwtsrc/qwt_picker_machine.h \
    ../Qwtsrc/qwt_panner.h \
    ../Qwtsrc/qwt_painter.h \
    ../Qwtsrc/qwt_null_paintdevice.h \
    ../Qwtsrc/qwt_matrix_raster_data.h \
    ../Qwtsrc/qwt_math.h \
    ../Qwtsrc/qwt_magnifier.h \
    ../Qwtsrc/qwt_legend.h \
    ../Qwtsrc/qwt_legend_itemmanager.h \
    ../Qwtsrc/qwt_legend_item.h \
    ../Qwtsrc/qwt_knob.h \
    ../Qwtsrc/qwt_interval.h \
    ../Qwtsrc/qwt_interval_symbol.h \
    ../Qwtsrc/qwt_global.h \
    ../Qwtsrc/qwt_event_pattern.h \
    ../Qwtsrc/qwt_dyngrid_layout.h \
    ../Qwtsrc/qwt_double_range.h \
    ../Qwtsrc/qwt_dial.h \
    ../Qwtsrc/qwt_dial_needle.h \
    ../Qwtsrc/qwt_curve_fitter.h \
    ../Qwtsrc/qwt_counter.h \
    ../Qwtsrc/qwt_compat.h \
    ../Qwtsrc/qwt_compass.h \
    ../Qwtsrc/qwt_compass_rose.h \
    ../Qwtsrc/qwt_column_symbol.h \
    ../Qwtsrc/qwt_color_map.h \
    ../Qwtsrc/qwt_clipper.h \
    ../Qwtsrc/qwt_arrow_button.h \
    ../Qwtsrc/qwt_analog_clock.h \
    ../Qwtsrc/qwt_abstract_slider.h \
    ../Qwtsrc/qwt_abstract_scale.h \
    ../Qwtsrc/qwt_abstract_scale_draw.h \
    rovdebug.h \
    rovsettings.h \
    rovmappings.h \
    rovabout.h

FORMS    += mainwindow.ui \
    rovdebug.ui \
    rovsettings.ui \
    rovmappings.ui \
    rovabout.ui

INCLUDEPATH += ../QActivityMonitor/ \
                ../QBoolMonitor/ \
                ../QROVMotor/ \
                ../QROVRelay/ \
                ../QROVServo/ \
                ../Qwtsrc/

RESOURCES += \
    resources.qrc
