/*    Copyright (c) 2010-2017, Delft University of Technology
*    All rigths reserved
*
*    This file is part of the Tudat. Redistribution and use in source and
*    binary forms, with or without modification, are permitted exclusively
*    under the terms of the Modified BSD license. You should have received
*    a copy of the license with this file. If not, please or visit:
*    http://tudat.tudelft.nl/LICENSE.
*/

#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "pagmo/algorithms/de1220.hpp"
#include "pagmo/algorithms/simulated_annealing.hpp"
#include "pagmo/algorithms/sade.hpp"
#include "pagmo/algorithms/cmaes.hpp"

#include "Problems/earthMarsTransfer.h"
#include "Problems/applicationOutput.h"
#include "Problems/getAlgorithm.h"
#include "Problems/saveOptimizationResults.h"


//! Execute  main
int main( )
{
    using namespace tudat_pagmo_applications;

    //Set seed for reproducible results
    pagmo::random_device::set_seed( 123 );

    // We have two decision variables each with a lower and upper bound, create a vector of vectors that will contain these.
    std::vector< std::vector< double > > bounds( 2, std::vector< double >( 2, 0.0 ) );

    // Define bounds: Search between 2020 and 2025 for flight duration between 200 and 1000 days.
    bounds[ 0 ][ 0 ] = 2458849.5;
    bounds[ 1 ][ 0 ] = 2460676.5;
    bounds[ 0 ][ 1 ] = 200;
    bounds[ 1 ][ 1 ] = 1000;

    // Create object to compute the problem fitness
    problem prob{EarthMarsTransfer( bounds )};

    // Perform grid saerch
    createGridSearch( prob, bounds, { 1000, 1000 }, "porkchopEarthMars" );

    // Perform optimization with 8 different optimizers
    for( int j = 0; j < 8; j++ )
    {
        // Retrieve algorothm
        int algorithmIndex = j;
        algorithm algo{getAlgorithm( algorithmIndex )};

        // Create an island with 1024 individuals
        island isl{algo, prob, 1024};

        // Evolve for 100 generations
        for( int i = 0 ; i < 100; i++ )
        {
            isl.evolve();
            while( isl.status()!=pagmo::evolve_status::idle )
                isl.wait();

            // Write current iteration results to file
            printPopulationToFile( isl.get_population( ).get_x( ), "earthMarsLambert_" + std::to_string( j ) + "_" + std::to_string( i ) , false );
            printPopulationToFile( isl.get_population( ).get_f( ), "earthMarsLambert_" + std::to_string( j ) + "_" + std::to_string( i ) , true );

            std::cout<<i<<" "<<algorithmIndex<<std::endl;
        }
    }

    return 0;

}
