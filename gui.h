// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef gui_h
#define gui_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>

class GUI {
public:
  GUI();
  Fl_Double_Window *uiw;
private:
  inline void cb_DBG_i(Fl_Button*, void*);
  static void cb_DBG(Fl_Button*, void*);
  inline void cb_NewButton_i(Fl_Return_Button*, void*);
  static void cb_NewButton(Fl_Return_Button*, void*);
  inline void pano_demo_button_i(Fl_Button*, void*);
  static void pano_demo_button(Fl_Button*, void*);
  inline void izb_demo_button_i(Fl_Button*, void*);
  static void izb_demo_button(Fl_Button*, void*);
  inline void reg_demo_button_i(Fl_Button*, void*);
  static void reg_demo_button(Fl_Button*, void*);
  inline void gps_demo_button_i(Fl_Button*, void*);
  static void gps_demo_button(Fl_Button*, void*);
public:
  void show();
  void DBG_cb();
  void NewButton_cb();
  void PanoDemo();
  void IZBDemo();
  void RegShadowDemo();
  void GPShadowDemo();
};
#endif
