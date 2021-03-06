/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDiscretizableColorTransferFunction.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkDiscretizableColorTransferFunction - a combination of vtkColorTransferFunction and
// vtkLookupTable.
// .SECTION Description
// This is a cross between a vtkColorTransferFunction and a vtkLookupTable
// selectively combiniting the functionality of both.
// NOTE: One must call Build() after making any changes to the points
// in the ColorTransferFunction to ensure that the discrete and non-discrete
// version match up.

#ifndef __vtkDiscretizableColorTransferFunction_h
#define __vtkDiscretizableColorTransferFunction_h

#include "vtkColorTransferFunction.h"

class vtkLookupTable;
class vtkColorTransferFunction;

class VTK_FILTERING_EXPORT vtkDiscretizableColorTransferFunction : public vtkColorTransferFunction
{
public:
  static vtkDiscretizableColorTransferFunction* New();
  vtkTypeMacro(vtkDiscretizableColorTransferFunction, vtkColorTransferFunction);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Generate discretized lookup table, if applicable.
  // This method must be called after changes to the ColorTransferFunction
  // otherwise the discretized version will be inconsitent with the 
  // non-discretized one.
  virtual void Build();

  // Description:
  // Set if the values are to mapped after discretization. The
  // number of discrete values is set by using SetNumberOfValues().
  // Not set by default, i.e. color value is determined by
  // interpolating at the scalar value.
  vtkSetMacro(Discretize, int);
  vtkGetMacro(Discretize, int);
  vtkBooleanMacro(Discretize, int);

  // Description:
  // Get/Set if log scale must be used while mapping scalars
  // to colors. The default is 0.
  virtual void SetUseLogScale(int useLogScale);
  vtkGetMacro(UseLogScale, int);

  // Description:
  // Set the number of values i.e. colors to be generated in the
  // discrete lookup table. This has no effect if Discretize is off.
  // The default is 256.
  void SetNumberOfValues(vtkIdType number);
  vtkGetMacro(NumberOfValues, vtkIdType);

  // Description:
  // Map one value through the lookup table and return a color defined
  // as a RGBA unsigned char tuple (4 bytes).
  virtual unsigned char *MapValue(double v);

  // Description:
  // Map one value through the lookup table and return the color as
  // an RGB array of doubles between 0 and 1.
  virtual void GetColor(double v, double rgb[3]);

  // Description:
  // An internal method maps a data array into a 4-component, unsigned char
  // RGBA array. The color mode determines the behavior of mapping. If 
  // VTK_COLOR_MODE_DEFAULT is set, then unsigned char data arrays are
  // treated as colors (and converted to RGBA if necessary); otherwise, 
  // the data is mapped through this instance of ScalarsToColors. The offset
  // is used for data arrays with more than one component; it indicates 
  // which component to use to do the blending.
  // When the component argument is -1, then the this object uses its
  // own selected technique to change a vector into a scalar to map.
  virtual vtkUnsignedCharArray *MapScalars(vtkDataArray *scalars, int colorMode,
                                   int component);

  // Description:
  // Returns the (x, r, g, b) values as an array.
  double* GetRGBPoints();

  // Description:
  // Specify an additional opacity (alpha) value to blend with. Values
  // != 1 modify the resulting color consistent with the requested
  // form of the output. This is typically used by an actor in order to
  // blend its opacity.
  // Overridden to pass the alpha to the internal vtkLookupTable.
  virtual void SetAlpha(double alpha);


  // Description:
  // This should return 1 is the subclass is using log scale for mapping scalars
  // to colors. 
  virtual int UsingLogScale()
    { return this->UseLogScale; }

protected:
  vtkDiscretizableColorTransferFunction();
  ~vtkDiscretizableColorTransferFunction();

  int Discretize;
  int UseLogScale;

  vtkIdType NumberOfValues;
  vtkLookupTable* LookupTable;

  vtkTimeStamp BuildTime;
private:
  vtkDiscretizableColorTransferFunction(const vtkDiscretizableColorTransferFunction&); // Not implemented.
  void operator=(const vtkDiscretizableColorTransferFunction&); // Not implemented.

  // Pointer used by GetRGBPoints().
  double* Data;
};

#endif

