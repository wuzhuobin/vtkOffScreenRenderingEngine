// me 
#include "vtkVolumeViewer.h"
// qt 
#include <QObject>
#include <QTest>
// vtk
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPNGWriter.h>
#include <vtkNIFTIImageReader.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkCommand.h>
// std 
#include <string>
#include <fstream>
#include <iostream>
void CreateImageData(vtkImageData* imageData)
{
  // Create a spherical implicit function.
  vtkSmartPointer<vtkSphere> sphere =
    vtkSmartPointer<vtkSphere>::New();
  sphere->SetRadius(100);
  sphere->SetCenter(0.0,0.0,0.0);
 
  vtkSmartPointer<vtkSampleFunction> sampleFunction =
    vtkSmartPointer<vtkSampleFunction>::New();
  sampleFunction->SetImplicitFunction(sphere);
  sampleFunction->SetOutputScalarTypeToDouble();
  sampleFunction->SetSampleDimensions(127,127,127); // intentional NPOT dimensions.
  sampleFunction->SetModelBounds(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  sampleFunction->SetCapping(false);
  sampleFunction->SetComputeNormals(false);
  sampleFunction->SetScalarArrayName("values");
  sampleFunction->Update();
 
  vtkDataArray* a = sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);
 
  vtkSmartPointer<vtkImageShiftScale> t = 
    vtkSmartPointer<vtkImageShiftScale>::New();
  t->SetInputConnection(sampleFunction->GetOutputPort());
 
  t->SetShift(-range[0]);
  double magnitude=range[1]-range[0];
  if(magnitude==0.0)
    {
    magnitude=1.0;
    }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToUnsignedChar();
 
  t->Update();
 
  imageData->ShallowCopy(t->GetOutput());
}

class TestvtkVolumeViewerOffScreenInteraction final: public QObject
{
  Q_OBJECT;
private Q_SLOTS:
  void initTestCase()
  {
    CreateImageData(image);
  }
  void cleanupTestCase(){}
  void init(){}
  void cleanup(){}
  void testInteraction()
  {
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = 
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleSwitch> style = 
      vtkSmartPointer<vtkInteractorStyleSwitch>::New();
    style->SetCurrentStyleToTrackballCamera();
    vtkSmartPointer<vtkVolumeViewer> viewer = 
      vtkSmartPointer<vtkVolumeViewer>::New();
    viewer->SetSize(500, 500);
    viewer->SetPreset(vtkVolumeViewer::CT_AAA);
    viewer->SetInputData(this->image);
    viewer->ResetCamera();
    viewer->ResetCameraClippingRange(); 
    viewer->Render();
    viewer->SetupInteractor(interactor);
    interactor->SetInteractorStyle(style);
    interactor->Initialize();
    // interactio
    interactor->SetEventInformation(0, 0);
    interactor->InvokeEvent(vtkCommand::MiddleButtonPressEvent, nullptr);
    interactor->SetEventInformation(0, 100);
    interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
    interactor->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, nullptr);
    viewer->Render();
    // std::cin.get();
  }
private:
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
};
QTEST_GUILESS_MAIN(TestvtkVolumeViewerOffScreenInteraction);
#include "TestvtkVolumeViewerOffScreenInteraction.moc"