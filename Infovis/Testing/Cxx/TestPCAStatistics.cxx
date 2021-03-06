/*
 * Copyright 2008 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */
// .SECTION Thanks
// Thanks to Philippe Pebay and David Thompson from Sandia National Laboratories 
// for implementing this test.

#include "vtkDoubleArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPCAStatistics.h"
#include "vtkStringArray.h"
#include "vtkTable.h"
#include "vtkTestUtilities.h"

#include "vtksys/SystemTools.hxx"

//=============================================================================
int TestPCAStatistics( int argc, char* argv[] )
{
  char* normScheme = vtkTestUtilities::GetArgOrEnvOrDefault(
    "-normalize-covariance", argc, argv, "VTK_NORMALIZE_COVARIANCE", "None" );
  int testStatus = 0;

  /* */
  double mingledData[] = 
    {
    46, 45,
    47, 49,
    46, 47,
    46, 46,
    47, 46,
    47, 49,
    49, 49,
    47, 45,
    50, 50,
    46, 46,
    51, 50,
    48, 48,
    52, 54,
    48, 47,
    52, 52,
    49, 49,
    53, 54,
    50, 50,
    53, 54,
    50, 52,
    53, 53,
    50, 51,
    54, 54,
    49, 49,
    52, 52,
    50, 51,
    52, 52,
    49, 47,
    48, 48,
    48, 50,
    46, 48,
    47, 47
    };
  int nVals = 32;

  const char m0Name[] = "M0";
  vtkDoubleArray* dataset1Arr = vtkDoubleArray::New();
  dataset1Arr->SetNumberOfComponents( 1 );
  dataset1Arr->SetName( m0Name );

  const char m1Name[] = "M1";
  vtkDoubleArray* dataset2Arr = vtkDoubleArray::New();
  dataset2Arr->SetNumberOfComponents( 1 );
  dataset2Arr->SetName( m1Name );

  const char m2Name[] = "M2";
  vtkDoubleArray* dataset3Arr = vtkDoubleArray::New();
  dataset3Arr->SetNumberOfComponents( 1 );
  dataset3Arr->SetName( m2Name );

  for ( int i = 0; i < nVals; ++ i )
    {
    int ti = i << 1;
    dataset1Arr->InsertNextValue( mingledData[ti] );
    dataset2Arr->InsertNextValue( mingledData[ti + 1] );
    dataset3Arr->InsertNextValue( i != 12 ? -1. : -1.001 );
    }

  vtkTable* datasetTable = vtkTable::New();
  datasetTable->AddColumn( dataset1Arr );
  dataset1Arr->Delete();
  datasetTable->AddColumn( dataset2Arr );
  dataset2Arr->Delete();
  datasetTable->AddColumn( dataset3Arr );
  dataset3Arr->Delete();

  // Set PCA statistics algorithm and its input data port
  vtkPCAStatistics* pcas = vtkPCAStatistics::New();

  // First verify that absence of input does not cause trouble
  cout << "## Verifying that absence of input does not cause trouble... ";
  pcas->Update();
  cout << "done.\n";

  // Prepare first test with data
  pcas->SetInput( vtkStatisticsAlgorithm::INPUT_DATA, datasetTable );
  pcas->SetNormalizationSchemeByName( normScheme );
  pcas->SetBasisSchemeByName( "FixedBasisEnergy" );
  pcas->SetFixedBasisEnergy( 1. - 1e-8 );

  datasetTable->Delete();

  // -- Select Column Pairs of Interest ( Learn Mode ) -- 
  pcas->SetColumnStatus( m0Name, 1 );
  pcas->SetColumnStatus( m1Name, 1 );
  pcas->RequestSelectedColumns();
  pcas->ResetAllColumnStates();
  pcas->SetColumnStatus( m0Name, 1 );
  pcas->SetColumnStatus( m1Name, 1 );
  pcas->SetColumnStatus( m2Name, 1 );
  pcas->SetColumnStatus( m2Name, 0 );
  pcas->SetColumnStatus( m2Name, 1 );
  pcas->RequestSelectedColumns();
  pcas->RequestSelectedColumns(); // Try a duplicate entry. This should have no effect.
  pcas->SetColumnStatus( m0Name, 0 );
  pcas->SetColumnStatus( m2Name, 0 );
  pcas->SetColumnStatus( "Metric 3", 1 ); // An invalid name. This should result in a request for metric 1's self-correlation.
  // pcas->RequestSelectedColumns(); will get called in RequestData()

  // Test all options but Assess
  pcas->SetLearnOption( true );
  pcas->SetDeriveOption( true );
  pcas->SetTestOption( true );
  pcas->SetAssessOption( false );
  pcas->Update();

  vtkMultiBlockDataSet* outputMetaDS = vtkMultiBlockDataSet::SafeDownCast( pcas->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  vtkTable* outputTest = pcas->GetOutput( vtkStatisticsAlgorithm::OUTPUT_TEST );

  cout << "## Calculated the following statistics for data set:\n";
  for ( unsigned int b = 0; b < outputMetaDS->GetNumberOfBlocks(); ++ b )
    {
    vtkTable* outputMeta = vtkTable::SafeDownCast( outputMetaDS->GetBlock( b ) );

    if ( b == 0 )
      {
      cout << "Primary Statistics\n";
      }
    else
      {
      cout << "Derived Statistics " << ( b - 1 ) << "\n";
      }

    outputMeta->Dump();
    }

  // Check some results of the Test option
  cout << "\n## Calculated the following Jarque-Bera-Srivastava statistics for pseudo-random variables (n="
       << nVals;

#ifdef VTK_USE_GNU_R
  int nNonGaussian = 1;
  int nRejected = 0;
  double alpha = .01;

  cout << ", null hypothesis: binormality, significance level="
       << alpha;
#endif // VTK_USE_GNU_R

  cout << "):\n";

  // Loop over Test table
  for ( vtkIdType r = 0; r < outputTest->GetNumberOfRows(); ++ r )
    {
    cout << "   ";
    for ( int i = 0; i < outputTest->GetNumberOfColumns(); ++ i )
      {
      cout << outputTest->GetColumnName( i )
           << "="
           << outputTest->GetValue( r, i ).ToString()
           << "  ";
      }

#ifdef VTK_USE_GNU_R
    // Check if null hypothesis is rejected at specified significance level
    double p = outputTest->GetValueByName( r, "P" ).ToDouble();
    // Must verify that p value is valid (it is set to -1 if R has failed)
    if ( p > -1 && p < alpha )
      {
      cout << "N.H. rejected";

      ++ nRejected;
      }
#endif // VTK_USE_GNU_R

    cout << "\n";
    }

#ifdef VTK_USE_GNU_R
  if ( nRejected < nNonGaussian )
    {
    vtkGenericWarningMacro("Rejected only "
                           << nRejected
                           << " null hypotheses of binormality whereas "
                           << nNonGaussian
                           << " variable pairs are not Gaussian");
    testStatus = 1;
    }
#endif // VTK_USE_GNU_R

  // Test Assess option
  vtkMultiBlockDataSet* paramsTables = vtkMultiBlockDataSet::New();
  paramsTables->ShallowCopy( outputMetaDS );

  pcas->SetInput( vtkStatisticsAlgorithm::INPUT_MODEL, paramsTables );
  paramsTables->Delete();

  // Test Assess only (Do not recalculate nor rederive nor retest a model)
  pcas->SetLearnOption( false );
  pcas->SetDeriveOption( false );
  pcas->SetTestOption( false );
  pcas->SetAssessOption( true );
  pcas->Update();

  cout << "\n## Assessment results:\n";
  vtkTable* outputData = pcas->GetOutput();
  outputData->Dump();

  pcas->Delete();
  delete [] normScheme;

  return testStatus;
}
