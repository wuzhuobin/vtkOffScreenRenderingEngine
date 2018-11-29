// me 
#include "vtk_volume_viewer_json_interpreter.h"
#include "vtkVolumeViewer.h"
// vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCommand.h>
// boost
using namespace boost::property_tree;
// std
#include <iostream>
#include <sstream>
using namespace std;
vtk_volume_viewer_json_interpreter::vtk_volume_viewer_json_interpreter(bool debug) :
  debug(debug),
  style(vtkInteractorStyleTrackballCamera::New())
{

}

vtk_volume_viewer_json_interpreter::~vtk_volume_viewer_json_interpreter()
{
  this->style->Delete();
}

void vtk_volume_viewer_json_interpreter::read_json(const std::string &content)
{
  try
  {
    stringstream ss;
    ss << content;
    boost::property_tree::read_json(ss, this->content);
  }
  catch (json_parser::json_parser_error &e)
  {
    cerr << e.what() << '\n';
    cerr << "Cannot parse this json.\n";
  }
}

void vtk_volume_viewer_json_interpreter::read_json_file(const std::string &filename)
{
  try
  {
    boost::property_tree::read_json(filename, this->content);
  }
  catch(json_parser::json_parser_error &e)
  {
    cerr << e.what() << '\n';
    cerr << "Cannot parse this json.\n";
  }
}

void vtk_volume_viewer_json_interpreter::write_json(std::string &content)
{
  stringstream ss;
  boost::property_tree::write_json(ss, this->content);
  content = ss.str();
}

void vtk_volume_viewer_json_interpreter::write_json_file(const std::string &filename)
{
  boost::property_tree::write_json(filename, this->content);
}

void vtk_volume_viewer_json_interpreter::interpret(vtkVolumeViewer *viewer) const 
{
  if(viewer == nullptr)
  {
      cerr << "viewer is nullptr\n" ;
      return;
  }
  int preset;
  if(this->get_value("preset", preset))
  {
    viewer->SetPreset(preset);
  }
  double shift;
  if(this->get_value("shift", shift))
  {
    viewer->SetShift(shift);
  }
  double opacity;
  if(this->get_value("opacity", opacity))
  {
      viewer->SetOpacity(opacity);
  }
  array<int, 2> size;
  if(this->get_values("size", size)){
    viewer->SetSize(size[0], size[1]);
  }
  this->interpret(viewer->GetInteractor());
}

void vtk_volume_viewer_json_interpreter::interpret(vtkRenderWindowInteractor *interactor) const 
{
  if(interactor == nullptr)
  {
    cerr << "interactor is nullptr. \n";
    return;
  }
  interactor->SetInteractorStyle(this->style);
  this->dolly(interactor);
  this->pan(interactor);
  this->spin(interactor);
  this->rotate(interactor);
  interactor->SetInteractorStyle(nullptr);
}

void vtk_volume_viewer_json_interpreter::dolly(vtkRenderWindowInteractor *interactor) const
{
  array<double, 2> current;
  array<double, 2> last;
  double motion_factor = 1;
  this->get_value("dolly.motionFactor", motion_factor);
  if(!this->get_values("dolly.current", current))
  {
    return;
  }
  if(!this->get_values("dolly.last", last))
  {
    return;
  }
  this->style->SetMotionFactor(motion_factor);
  interactor->SetEventInformationFlipY(last[0], last[1], 1, 1);
  interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
  interactor->SetEventInformationFlipY(current[0], current[1], 1, 1);
  interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
  interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
}

void vtk_volume_viewer_json_interpreter::pan(vtkRenderWindowInteractor *interactor) const
{
  array<double, 2> current;
  array<double, 2> last;
  if (!this->get_values<double, 2>("pan.current", current))
  {
    return;
  }
  if (!this->get_values<double, 2>("pan.last", last))
  {
    return;
  }
  interactor->SetEventInformation(last[0], last[1]);
  interactor->SetEventInformationFlipY(last[0], last[1], 0, 1);
  interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
  interactor->SetEventInformationFlipY(current[0], current[1], 0, 1);
  interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
  interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
}

void vtk_volume_viewer_json_interpreter::spin(vtkRenderWindowInteractor *interactor) const
{
  array<double, 2> current;
  array<double, 2> last;
  if (!this->get_values<double, 2>("spin.current", current))
  {
    return;
  }
  if (!this->get_values<double, 2>("spin.last", last))
  {
    return;
  }
  interactor->SetEventInformationFlipY(last[0], last[1], 1, 0);
  interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
  interactor->SetEventInformationFlipY(current[0], current[1], 1, 0);
  interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
  interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
}

void vtk_volume_viewer_json_interpreter::rotate(vtkRenderWindowInteractor *interactor) const
{
  array<double, 2> current;
  array<double, 2> last;
  double motion_factor = 1;
  this->get_value("rotate.motionFactor", motion_factor);
  if (!this->get_values<double, 2>("rotate.current", current))
  {
    return;
  }
  if (!this->get_values<double, 2>("rotate.last", last))
  {
    return;
  }
  this->style->SetMotionFactor(motion_factor);
  interactor->SetEventInformationFlipY(last[0], last[1], 0, 0);
  interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
  interactor->SetEventInformationFlipY(current[0], current[1], 0, 0);
  interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
  interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
}

template<typename T>
bool vtk_volume_viewer_json_interpreter::get_value(const std::string &key, T &value) const
{
  try
  {
    const ptree &child = this->content.get_child(key);
    T attribute = child.get_value<T>();
    value = attribute;
  }
  catch(ptree_bad_path &e)
  {
    if(debug)
    {
      cerr << e.what() << '\n';
      cerr << "The json file does not contain this child: " << key << '\n';
    }
    return false;
  }
  catch(ptree_bad_data &e)
  {
    if(debug)
    {
      cerr << e.what() << '\n';
      cerr << "The key does not contain value. \n";
    }
    return false;
  }
  return true;
}

template<typename T, size_t N>
bool vtk_volume_viewer_json_interpreter::get_values(const std::string &key, std::array<T, N>& values) const
{
  try
  {
    const ptree &child = this->content.get_child(key);
    typename array<T, N>::iterator it_v = values.begin();
    typename ptree::const_iterator it_p = child.begin();
    while (it_v != values.cend() && it_p != child.end())
    {
      *it_v = it_p->second.get_value<T>();
      ++it_p;
      ++it_v;
    }
  }
  catch (ptree_bad_path &e)
  {
    if(debug)
    {
      cerr << e.what() << '\n';
      cerr << "The json file does not contain this child: " << key << '\n';
    }
    return false;
  }
  catch (ptree_bad_data &e)
  {
    if(debug)
    {
      cerr << e.what() << '\n';
      cerr << "The key does not contain value. \n";
    }
    return false;
  }
  return true;
}