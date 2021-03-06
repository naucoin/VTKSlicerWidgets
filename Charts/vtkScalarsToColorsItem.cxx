/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkScalarsToColorsItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkBrush.h"
#include "vtkCallbackCommand.h"
#include "vtkContext2D.h"
#include "vtkContextScene.h"
#include "vtkImageData.h"
#include "vtkScalarsToColorsItem.h"
#include "vtkObjectFactory.h"
#include "vtkPen.h"
#include "vtkPoints2D.h"
#include "vtkSmartPointer.h"
#include "vtkTransform2D.h"

#include <cassert>

//-----------------------------------------------------------------------------
vtkScalarsToColorsItem::vtkScalarsToColorsItem()
{
  this->Pen->SetWidth(0.0);
  this->Pen->SetLineType(vtkPen::NO_PEN);

  this->Texture = 0;
  this->Interpolate = true;
  this->Shape = vtkPoints2D::New();
  this->Shape->SetDataTypeToFloat();
  this->Shape->SetNumberOfPoints(4);
  this->Shape->SetPoint(0, 0.f, 0.f);
  this->Shape->SetPoint(1, 1.f, 0.f);
  this->Shape->SetPoint(2, 1.f, 1.f);
  this->Shape->SetPoint(3, 0.f, 1.f);

  this->Callback = vtkCallbackCommand::New();
  this->Callback->SetClientData(this);
  this->Callback->SetCallback(
    vtkScalarsToColorsItem::OnScalarsToColorsModified);
}

//-----------------------------------------------------------------------------
vtkScalarsToColorsItem::~vtkScalarsToColorsItem()
{
  if (this->Texture)
    {
    this->Texture->Delete();
    this->Texture = 0;
    }
  if (this->Shape)
    {
    this->Shape->Delete();
    this->Shape = 0;
    }
  if (this->Callback)
    {
    this->Callback->Delete();
    this->Callback = 0;
    }
}

//-----------------------------------------------------------------------------
void vtkScalarsToColorsItem::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Interpolate: " << this->Interpolate << endl;
}

//-----------------------------------------------------------------------------
void vtkScalarsToColorsItem::GetBounds(double bounds[4])
{
  this->Shape->GetBounds(bounds);
}

//-----------------------------------------------------------------------------
bool vtkScalarsToColorsItem::Paint(vtkContext2D* painter)
{
  if (this->Texture == 0 ||
      this->Texture->GetMTime() < this->GetMTime())
    {
    this->ComputeTexture();
    }
  painter->ApplyPen(this->Pen);
  painter->GetBrush()->SetColorF(1., 1., 1., 1.);
  painter->GetBrush()->SetTexture(this->Texture);
  painter->GetBrush()->SetTextureProperties(
    (this->Interpolate ? vtkBrush::Nearest : vtkBrush::Linear) |
    vtkBrush::Stretch);

  painter->DrawPolygon(this->Shape);

  return true;
}

//-----------------------------------------------------------------------------
void vtkScalarsToColorsItem::OnScalarsToColorsModified(vtkObject* caller,
                                                       unsigned long eid,
                                                       void *clientdata,
                                                       void* calldata)
{
  vtkScalarsToColorsItem* self =
    reinterpret_cast<vtkScalarsToColorsItem*>(clientdata);
  self->ScalarsToColorsModified(caller, eid, calldata);
}

//-----------------------------------------------------------------------------
void vtkScalarsToColorsItem::ScalarsToColorsModified(vtkObject* vtkNotUsed(object),
                                                     unsigned long vtkNotUsed(eid),
                                                     void* vtkNotUsed(calldata))
{
  this->Modified();
}
