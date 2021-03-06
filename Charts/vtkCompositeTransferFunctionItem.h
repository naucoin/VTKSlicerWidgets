/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCompositeTransferFunctionItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkCompositeTransferFunctionItem_h
#define __vtkCompositeTransferFunctionItem_h

#include "vtkColorTransferFunctionItem.h"

class vtkPiecewiseFunction;

class VTK_CHARTS_EXPORT vtkCompositeTransferFunctionItem: public vtkColorTransferFunctionItem
{
public:
  static vtkCompositeTransferFunctionItem* New();
  vtkTypeMacro(vtkCompositeTransferFunctionItem, vtkColorTransferFunctionItem);
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  void SetOpacityFunction(vtkPiecewiseFunction* opacity);
  vtkGetObjectMacro(OpacityFunction, vtkPiecewiseFunction);

  void SetMaskAboveCurve(bool mask);
  vtkGetMacro(MaskAboveCurve, bool);
protected:
  vtkCompositeTransferFunctionItem();
  virtual ~vtkCompositeTransferFunctionItem();

  virtual void ComputeTexture();
  virtual void ScalarsToColorsModified(vtkObject* object,
                                       unsigned long eid,
                                       void* calldata);
  vtkPiecewiseFunction* OpacityFunction;
  bool                  MaskAboveCurve;

private:
  vtkCompositeTransferFunctionItem(const vtkCompositeTransferFunctionItem&); // Not implemented.
  void operator=(const vtkCompositeTransferFunctionItem&); // Not implemented
};

#endif
