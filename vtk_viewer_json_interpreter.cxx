// me 
#include "vtk_viewer_json_interpreter.h"
#include "vtkVolumeViewer.h"
// vtk
#include <vtkRenderer.h>
#include <vtkCamera.h>
// boost
using namespace boost::property_tree;
// std
#include <iostream>
#include <sstream>
using namespace std;
vtk_viewer_json_interpreter::vtk_viewer_json_interpreter(bool debug) :
  debug(debug)
{

}

vtk_viewer_json_interpreter::~vtk_viewer_json_interpreter()
{
}

void vtk_viewer_json_interpreter::read_json(const std::string &content)
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

void vtk_viewer_json_interpreter::read_json_file(const std::string &filename)
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

void vtk_viewer_json_interpreter::write_json(std::string &content)
{
  stringstream ss;
  boost::property_tree::write_json(ss, this->content);
  content = ss.str();
}

void vtk_viewer_json_interpreter::write_json_file(const std::string &filename)
{
  boost::property_tree::write_json(filename, this->content);
}

void vtk_viewer_json_interpreter::interpret(vtkVolumeViewer *viewer) const 
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

void vtk_viewer_json_interpreter::interpret(vtkRenderWindowInteractor *interactor) const 
{
  if(interactor == nullptr)
  {
    cerr << "renderer is nullptr. \n";
    return;
  }
}

template<typename T>
bool vtk_viewer_json_interpreter::get_value(const std::string &key, T &value) const
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
bool vtk_viewer_json_interpreter::get_values(const std::string &key, std::array<T, N>& values) const
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