//Tests for constitutive_tools

#include<micromorphic_linear_elasticity.h>
#include<sstream>
#include<fstream>
#include<iostream>

typedef micromorphicTools::constantType constantType;
typedef micromorphicTools::constantVector constantVector;
typedef micromorphicTools::constantMatrix constantMatrix;

typedef micromorphicTools::parameterType parameterType;
typedef micromorphicTools::parameterVector parameterVector;
typedef micromorphicTools::parameterMatrix parameterMatrix;

typedef micromorphicTools::variableType variableType;
typedef micromorphicTools::variableVector variableVector;
typedef micromorphicTools::variableMatrix variableMatrix;

typedef micromorphicTools::errorNode errorNode;
typedef micromorphicTools::errorOut errorOut;

struct cout_redirect{
    cout_redirect( std::streambuf * new_buffer)
        : old( std::cout.rdbuf( new_buffer ) )
    { }

    ~cout_redirect( ) {
        std::cout.rdbuf( old );
    }

    private:
        std::streambuf * old;
};

struct cerr_redirect{
    cerr_redirect( std::streambuf * new_buffer)
        : old( std::cerr.rdbuf( new_buffer ) )
    { }

    ~cerr_redirect( ) {
        std::cerr.rdbuf( old );
    }

    private:
        std::streambuf * old;
};

int test_computeDeformationMeasures( std::ofstream &results ){
    /*!
     * Test the computation of the deformation metrics.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector deformationGradient = { -0.50668478, -0.48303615, -1.43907185,
                                            0.24106815,  0.10656166,  1.06851718,
                                           -0.18353482, -1.11676646, -0.68534721 };

    variableVector microDeformation = { -0.36302711,  0.94624033,  0.07877948,
                                        -0.68872716,  0.10276167, -0.81357538,
                                         0.22307023, -0.23788599,  0.67759487 };

    variableVector gradientMicroDeformation = { 0.80507219, 0.73460211, 0.66571977, 0.13571332, 0.18559912,
                                                0.99230253, 0.77887526, 0.23648914, 0.31711178, 0.75118698,
                                                0.08013972, 0.27232507, 0.59595994, 0.13892773, 0.51533812,
                                                0.19823639, 0.51598785, 0.19048906, 0.45974189, 0.01871104,
                                                0.51255207, 0.82869552, 0.99152216, 0.51920895, 0.06818867,
                                                0.12194391, 0.32637525 };

    variableVector answerC = { 0.34852835, 0.47540122, 1.11252634,
                               0.47540122, 1.49184663, 1.57435946,
                               1.11252634, 1.57435946, 3.68235756 };

    variableVector answerPsi = { -0.02303102, -0.41101265, -0.36040573,
                                 -0.14715403, -0.18045474, -0.8814645 ,
                                 -0.36637526, -1.08887072, -1.44707636 };

    variableVector answerGamma = { -0.31120922, -0.3563267 , -0.36573233, -0.0771914 , -0.24252804,
                                   -0.4738459 , -0.35937075, -0.01781817, -0.17465609, -0.82225557,
                                   -0.36719542, -0.86494826, -0.92750732, -1.18214541, -1.00423785,
                                   -0.43125133, -0.19543115, -0.49736256, -0.67098335, -0.98433811,
                                   -1.0183107 , -0.12645195, -0.79818076, -1.23318541, -0.95577138,
                                    0.1274431 , -0.47648617 };

    variableVector resultC, resultPsi, resultGamma;
    errorOut error = micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient, microDeformation, gradientMicroDeformation,
                                                                               resultC, resultPsi, resultGamma );

    if ( error ){
        error->print();
        results << "test_computeDeformationMeasures & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultC, answerC ) ){
        results << "test_computeDeformationMeasures (test 1) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultPsi, answerPsi ) ){
        results << "test_computeDeformationMeasures (test 2) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultGamma, answerGamma ) ){
        results << "test_computeDeformationMeasures (test 3) & False\n";
        return 1;
    }

    //Test the jacobians

    variableVector resultCJ, resultPsiJ, resultGammaJ;
    variableMatrix dCdF, dPsidF, dPsidXi, dGammadF, dGammadGradXi;

    error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient, microDeformation, gradientMicroDeformation,
                                                                       resultCJ, resultPsiJ, resultGammaJ, dCdF, dPsidF, dPsidXi,
                                                                       dGammadF, dGammadGradXi );

    if ( error ){
        error->print();
        results << "test_computeDeformationMeasures & False\n";
    }

    if ( !vectorTools::fuzzyEquals( resultCJ, answerC ) ){
        results << "test_computeDeformationMeasures (test 4) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultPsiJ, answerPsi ) ){
        results << "test_computeDeformationMeasures (test 5) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultGammaJ, answerGamma ) ){
        results << "test_computeDeformationMeasures (test 6) & False\n";
        return 1;
    }

    //Test jacobians w.r.t. the deformation gradient
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < deformationGradient.size(); i++ ){
        constantVector delta( deformationGradient.size(), 0 );
        delta[i] = eps * fabs( deformationGradient[i] ) + eps;

        variableVector resultC_P, resultC_M;
        variableVector resultPsi_P, resultPsi_M;
        variableVector resultGamma_P, resultGamma_M;

        error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient + delta, microDeformation, 
                                                                           gradientMicroDeformation,
                                                                           resultC_P, resultPsi_P, resultGamma_P );

        if ( error ){
            error->print();
            results << "test_computeDeformationMeasures & False\n";
            return 1;
        }

        error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient - delta, microDeformation, 
                                                                           gradientMicroDeformation,
                                                                           resultC_M, resultPsi_M, resultGamma_M );

        if ( error ){
            error->print();
            results << "test_computeDeformationMeasures & False\n";
            return 1;
        }

        variableVector gradCol = ( resultC_P - resultC_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dCdF[j][i] ) ){
                results << "test_computeDeformationMeasures (test 7) & False\n";
                return 1;
            }
        }

        gradCol = ( resultPsi_P - resultPsi_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dPsidF[j][i] ) ){
                results << "test_computeDeformationMeasures (test 8) & False\n";
                return 1;
            }
        }

        gradCol = ( resultGamma_P - resultGamma_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dGammadF[j][i] ) ){
                results << "test_computeDeformationMeasures (test 9) & False\n";
                return 1;
            }
        }
    }

    //Test jacobians w.r.t. the micro deformation
    for ( unsigned int i = 0; i < microDeformation.size(); i++ ){
        constantVector delta( microDeformation.size(), 0 );
        delta[i] = eps * fabs( microDeformation[i] ) + eps;

        variableVector resultC_P, resultC_M;
        variableVector resultPsi_P, resultPsi_M;
        variableVector resultGamma_P, resultGamma_M;

        error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient, microDeformation + delta, 
                                                                           gradientMicroDeformation,
                                                                           resultC_P, resultPsi_P, resultGamma_P );

        if ( error ){
            error->print();
            results << "test_computeDeformationMeasures & False\n";
            return 1;
        }

        error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient, microDeformation - delta, 
                                                                           gradientMicroDeformation,
                                                                           resultC_M, resultPsi_M, resultGamma_M );

        if ( error ){
            error->print();
            results << "test_computeDeformationMeasures & False\n";
            return 1;
        }

        variableVector gradCol = ( resultC_P - resultC_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_computeDeformationMeasures (test 10) & False\n";
            }
        }

        gradCol = ( resultPsi_P - resultPsi_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dPsidXi[j][i] ) ){
                results << "test_computeDeformationMeasures (test 11) & False\n";
                return 1;
            }
        }

        gradCol = ( resultGamma_P - resultGamma_M ) / ( 2 * delta[ i ] );
        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], 0. ) ){
                results << "test_computeDeformationMeasures (test 12) * False\n";
                return 1;
            }
        }
    }

    //Test jacobians w.r.t. the gradient of the micro deformation
    for ( unsigned int i = 0; i < gradientMicroDeformation.size(); i++ ){
        constantVector delta( gradientMicroDeformation.size(), 0 );
        delta[i] = eps * fabs( gradientMicroDeformation[i] ) + eps;

        variableVector resultC_P, resultC_M;
        variableVector resultPsi_P, resultPsi_M;
        variableVector resultGamma_P, resultGamma_M;

        error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient, microDeformation,
                                                                           gradientMicroDeformation + delta,
                                                                           resultC_P, resultPsi_P, resultGamma_P );

        if ( error ){
            error->print();
            results << "test_computeDeformationMeasures & False\n";
            return 1;
        }

        error =  micromorphicLinearElasticity::computeDeformationMeasures( deformationGradient, microDeformation,
                                                                           gradientMicroDeformation - delta,
                                                                           resultC_M, resultPsi_M, resultGamma_M );

        if ( error ){
            error->print();
            results << "test_computeDeformationMeasures & False\n";
            return 1;
        }

        variableVector gradCol = ( resultC_P - resultC_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_computeDeformationMeasures (test 13) & False\n";
            }
        }

        gradCol = ( resultPsi_P - resultPsi_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_computeDeformationMeasures (test 14) & False\n";
                return 1;
            }
        }

        gradCol = ( resultGamma_P - resultGamma_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dGammadGradXi[j][i] ) ){
                results << "test_computeDeformationMeasures (test 15) & False\n";
                return 1;
            }
        }
    }



    results << "test_computeDeformationMeasures & True\n";
    return 0;
}

int test_computeLinearElasticTerm1( std::ofstream &results ){
    /*!
     * Test the computation of the linear elastic term 1
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector greenLagrangeStrain = { -0.32573583,  0.23770061,  0.55626317,
                                            0.23770061,  0.24592331,  0.78717973,
                                            0.55626317,  0.78717973,  1.34117878 };

    variableVector microStrain = { -1.02303102, -0.41101265, -0.36040573,
                                   -0.14715403, -1.18045474, -0.8814645 ,
                                   -0.36637526, -1.08887072, -2.44707636 };

    variableVector A = { 0.91738548, 0.5223949 , 0.04303308, 0.42138619, 0.71420722,
                         0.15443589, 0.05041408, 0.69624665, 0.8766614 , 0.17261697,
                         0.02350474, 0.59406857, 0.79573586, 0.21213138, 0.47821637,
                         0.35462425, 0.6291708 , 0.48565385, 0.67132896, 0.27926608,
                         0.04579313, 0.88556864, 0.08992741, 0.75316186, 0.76279627,
                         0.5635193 , 0.18529158, 0.05722408, 0.65275234, 0.97189144,
                         0.68435   , 0.96624106, 0.84374092, 0.21040392, 0.13887068,
                         0.34423717, 0.50801461, 0.28726825, 0.52590869, 0.36090934,
                         0.97602275, 0.95087184, 0.32716562, 0.50074403, 0.3955299 ,
                         0.48018626, 0.71150853, 0.61899609, 0.57103915, 0.5154469 ,
                         0.4456661 , 0.41192121, 0.12649935, 0.69069678, 0.17931527,
                         0.98427378, 0.82378172, 0.44572395, 0.90859147, 0.33180413,
                         0.30215348, 0.42929583, 0.61595281, 0.66534843, 0.31552903,
                         0.99326382, 0.87708958, 0.27827411, 0.30275486, 0.3209769 ,
                         0.81059907, 0.83577572, 0.54758756, 0.30482114, 0.00502004,
                         0.09242907, 0.24196602, 0.00779042, 0.04284832, 0.56224798,
                         0.86563423 };

    variableVector D = { 0.146517  , 0.3707709 , 0.08328664, 0.49256865, 0.69405928,
                         0.61837293, 0.47376479, 0.11188706, 0.2228905 , 0.36340832,
                         0.0693854 , 0.10615699, 0.88785036, 0.6677967 , 0.63177135,
                         0.57125641, 0.80023305, 0.08298528, 0.91838322, 0.18315431,
                         0.89992512, 0.53899603, 0.41261589, 0.31495081, 0.83742576,
                         0.09300794, 0.82360698, 0.21493177, 0.1629844 , 0.21152065,
                         0.16961513, 0.4914438 , 0.50520605, 0.14553687, 0.26358359,
                         0.75964966, 0.65752746, 0.71537866, 0.0052241 , 0.96884752,
                         0.39184445, 0.9992278 , 0.82985355, 0.77985287, 0.82259158,
                         0.40209148, 0.65923576, 0.86734155, 0.82929213, 0.45634802,
                         0.27689889, 0.96708886, 0.1655354 , 0.89114231, 0.19536992,
                         0.33743959, 0.73300973, 0.363747  , 0.26554793, 0.61940794,
                         0.68798717, 0.31331865, 0.29169741, 0.94606427, 0.57961661,
                         0.9410199 , 0.09121453, 0.42521405, 0.67622819, 0.69165347,
                         0.84437122, 0.21535456, 0.15999728, 0.34674508, 0.86254606,
                         0.04035766, 0.19320383, 0.05731681, 0.95023339, 0.38534862,
                         0.01313969 };

    variableVector answer = { -0.61146593, -0.95665152, -2.79159046,
                              -1.18310778, -3.24431984, -2.38955177,
                              -0.26626611, -1.49288592, -0.08960274 };

    variableVector result;
    errorOut error = micromorphicLinearElasticity::computeLinearElasticTerm1( greenLagrangeStrain, microStrain, 
                                                                              A, D, result );

    if ( error ){
        results << "test_computeLinearElasticTerm1 & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( result, answer ) ){
        results << "test_computeLinearElasticTerm1 (test 1) & False\n";
        return 1;
    }

    //Test on the Jacobians
    variableVector resultJ;
    variableMatrix dTerm1dGreenLagrangeStrain, dTerm1dMicroStrain;

    error = micromorphicLinearElasticity::computeLinearElasticTerm1( greenLagrangeStrain, microStrain, 
                                                                     A, D, resultJ,
                                                                     dTerm1dGreenLagrangeStrain,
                                                                     dTerm1dMicroStrain );

    if ( error ){
        results << "test_computeLinearElasticTerm1 & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJ, answer ) ){
        results << "test_computeLinearElasticTerm1 (test 2) & False\n";
        return 1;
    }

    //Test dTerm1dGreenLagrangeStrain
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < greenLagrangeStrain.size(); i++ ){
        constantVector delta( greenLagrangeStrain.size(), 0 );
        delta[i] = eps * fabs( greenLagrangeStrain[i] ) + eps;

        variableVector result_P, result_M;

        error =  micromorphicLinearElasticity::computeLinearElasticTerm1( greenLagrangeStrain + delta, microStrain, 
                                                                              A, D, result_P );

        if ( error ){
            results << "test_computeLinearElasticTerm1 & False\n";
            return 1;
        }

        error =  micromorphicLinearElasticity::computeLinearElasticTerm1( greenLagrangeStrain - delta, microStrain, 
                                                                              A, D, result_M );

        if ( error ){
            results << "test_computeLinearElasticTerm1 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm1dGreenLagrangeStrain[j][i] ) ){
                results << "test_computeLinearElasticTerm1 (test 3) & False\n";
                return 1;
            }
        }
    }

    for ( unsigned int i = 0; i < microStrain.size(); i++ ){
        constantVector delta( microStrain.size(), 0 );
        delta[i] = eps * fabs( microStrain[i] ) + eps;

        variableVector result_P, result_M;

        error =  micromorphicLinearElasticity::computeLinearElasticTerm1( greenLagrangeStrain, microStrain + delta, 
                                                                              A, D, result_P );

        if ( error ){
            results << "test_computeLinearElasticTerm1 & False\n";
            return 1;
        }

        error =  micromorphicLinearElasticity::computeLinearElasticTerm1( greenLagrangeStrain, microStrain - delta, 
                                                                              A, D, result_M );

        if ( error ){
            results << "test_computeLinearElasticTerm1 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm1dMicroStrain[j][i] ) ){
                results << "test_computeLinearElasticTerm1 (test 4) & False\n";
                return 1;
            }
        }
    }

    results << "test_computeLinearElasticTerm1 & True\n";
    return 0;
}

int test_computeLinearElasticTerm2( std::ofstream &results){
    /*!
     * Test the computation of term 2 for linear elasticity.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector greenLagrangeStrain = { -0.32573583,  0.23770061,  0.55626317,
                                            0.23770061,  0.24592331,  0.78717973,
                                            0.55626317,  0.78717973,  1.34117878 };

    variableVector microStrain = { -1.02303102, -0.41101265, -0.36040573,
                                   -0.14715403, -1.18045474, -0.8814645 ,
                                   -0.36637526, -1.08887072, -2.44707636};

    variableVector invCPsi = { 7.06496448, -6.60478112,  6.18226067,
                               0.01374041,  0.34618158, -0.31907041,
                              -2.23986034,  1.55175262, -2.12436531 };

    parameterVector B = { 0.99402085, 0.66339725, 0.73962847, 0.75991152, 0.91213988,
                          0.94984965, 0.6011931 , 0.32834193, 0.21231827, 0.65420996,
                          0.66533091, 0.83293238, 0.2537865 , 0.57946922, 0.79358565,
                          0.92885037, 0.0923514 , 0.12441041, 0.87678012, 0.87730359,
                          0.59116472, 0.21901437, 0.45976152, 0.86524067, 0.06668473,
                          0.83812813, 0.06951684, 0.91636315, 0.07878975, 0.3887551 ,
                          0.86632579, 0.84909984, 0.72558761, 0.12280263, 0.92078995,
                          0.48305302, 0.19393044, 0.82984994, 0.27981095, 0.60669024,
                          0.25483571, 0.2663953 , 0.3504269 , 0.50945399, 0.15647713,
                          0.46803744, 0.44503108, 0.86396469, 0.44057713, 0.97430525,
                          0.12593544, 0.48379355, 0.82188636, 0.31297267, 0.21986847,
                          0.8032502 , 0.07287772, 0.6974731 , 0.97608146, 0.49354539,
                          0.97886936, 0.16204678, 0.79205693, 0.81275365, 0.44670719,
                          0.22577849, 0.14381145, 0.92846116, 0.13905519, 0.36151037,
                          0.71576657, 0.5462745 , 0.41204395, 0.79415741, 0.73346637,
                          0.51829857, 0.31806782, 0.73860407, 0.21137896, 0.06281619,
                          0.08517056 };
 
    parameterVector D = { 0.146517  , 0.3707709 , 0.08328664, 0.49256865, 0.69405928,
                          0.61837293, 0.47376479, 0.11188706, 0.2228905 , 0.36340832,
                          0.0693854 , 0.10615699, 0.88785036, 0.6677967 , 0.63177135,
                          0.57125641, 0.80023305, 0.08298528, 0.91838322, 0.18315431,
                          0.89992512, 0.53899603, 0.41261589, 0.31495081, 0.83742576,
                          0.09300794, 0.82360698, 0.21493177, 0.1629844 , 0.21152065,
                          0.16961513, 0.4914438 , 0.50520605, 0.14553687, 0.26358359,
                          0.75964966, 0.65752746, 0.71537866, 0.0052241 , 0.96884752,
                          0.39184445, 0.9992278 , 0.82985355, 0.77985287, 0.82259158,
                          0.40209148, 0.65923576, 0.86734155, 0.82929213, 0.45634802,
                          0.27689889, 0.96708886, 0.1655354 , 0.89114231, 0.19536992,
                          0.33743959, 0.73300973, 0.363747  , 0.26554793, 0.61940794,
                          0.68798717, 0.31331865, 0.29169741, 0.94606427, 0.57961661,
                          0.9410199 , 0.09121453, 0.42521405, 0.67622819, 0.69165347,
                          0.84437122, 0.21535456, 0.15999728, 0.34674508, 0.86254606,
                          0.04035766, 0.19320383, 0.05731681, 0.95023339, 0.38534862,
                          0.01313969 };

    variableVector answer = { -9.86078237,  -0.45761677,   4.03889815,
                             -34.37720709,   0.44579995,  11.76941682,
                               5.79045315,  -0.72742985,  -0.30143069 };

    variableVector result;

    errorOut error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain, microStrain, invCPsi, 
                                                                              B, D, result );

    if ( error ){
        error->print();
        results << "test_computeLinearElasticTerm2 & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( result, answer) ){
        vectorTools::print( result );
        vectorTools::print( answer );
        results << "test_computeLinearElasticTerm2 (test 1) & False\n";
        return 1;
    }

    //Test the Jacobians

    variableVector resultJ;
    variableMatrix dTerm2dE, dTerm2dMicroE, dTerm2dInvCPsi;

    error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain, microStrain, invCPsi,
                                                                     B, D, resultJ, dTerm2dE, dTerm2dMicroE, dTerm2dInvCPsi );

    if ( error ){
        error->print();
        results << "test_computeLinearElasticTerm2 & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJ, answer) ){
        vectorTools::print( result );
        vectorTools::print( answer );
        results << "test_computeLinearElasticTerm2 (test 2) & False\n";
        return 1;
    }

    //Test dTerm2dE
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < greenLagrangeStrain.size(); i++ ){
        constantVector delta( greenLagrangeStrain.size(), 0 );
        delta[i] = eps * fabs( greenLagrangeStrain[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain + delta, microStrain, invCPsi,
                                                                              B, D, result_P );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm2 & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain - delta, microStrain, invCPsi,
                                                                              B, D, result_M );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm2 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm2dE[j][i] ) ){
                results << "test_computeLinearElasticTerm2 (test 3) & False\n";
                return 1;
            }
        }
    }

    //Test dTerm2dMicroE
    for ( unsigned int i = 0; i < microStrain.size(); i++ ){
        constantVector delta( microStrain.size(), 0 );
        delta[i] = eps * fabs( microStrain[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain, microStrain + delta, invCPsi,
                                                                              B, D, result_P );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm2 & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain, microStrain - delta, invCPsi,
                                                                              B, D, result_M );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm2 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm2dMicroE[j][i] ) ){
                results << "test_computeLinearElasticTerm2 (test 4) & False\n";
                return 1;
            }
        }
    }

    //Test dTerm2dInvCPsi
    for ( unsigned int i = 0; i < invCPsi.size(); i++ ){
        constantVector delta( invCPsi.size(), 0 );
        delta[i] = eps * fabs( invCPsi[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain, microStrain, invCPsi + delta,
                                                                              B, D, result_P );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm2 & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeLinearElasticTerm2( greenLagrangeStrain, microStrain, invCPsi - delta,
                                                                              B, D, result_M );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm2 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm2dInvCPsi[j][i] ) ){
                results << "test_computeLinearElasticTerm2 (test 4) & False\n";
                return 1;
            }
        }
    }

    results << "test_computeLinearElasticTerm2 & True\n";
    return 0;
}

int test_computeReferenceHigherOrderStress( std::ofstream &results ){
    /*!
     * Test the computation of the higher order stress in the reference configuration.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector Gamma = { -0.31120922, -0.3563267 , -0.36573233, -0.0771914 , -0.24252804,
                             -0.4738459 , -0.35937075, -0.01781817, -0.17465609, -0.82225557,
                             -0.36719542, -0.86494826, -0.92750732, -1.18214541, -1.00423785,
                             -0.43125133, -0.19543115, -0.49736256, -0.67098335, -0.98433811,
                             -1.0183107 , -0.12645195, -0.79818076, -1.23318541, -0.95577138,
                              0.1274431 , -0.47648617 };

    parameterVector C = { 0.73168423, 0.20787282, 0.44597068, 0.13971472, 0.67623962,
                          0.01024113, 0.26235898, 0.52638878, 0.55822266, 0.79169357,
                          0.79556295, 0.26274848, 0.22158735, 0.77447856, 0.09905053,
                          0.24537506, 0.82422833, 0.13912553, 0.13704714, 0.60418098,
                          0.97916951, 0.96975567, 0.15156735, 0.33820056, 0.40984014,
                          0.03316931, 0.07805217, 0.09086063, 0.0587449 , 0.93973718,
                          0.2088402 , 0.27030923, 0.90893679, 0.45495913, 0.38558114,
                          0.89599555, 0.99117823, 0.68663521, 0.47807759, 0.47462775,
                          0.63545614, 0.76359775, 0.58543208, 0.67889697, 0.23400169,
                          0.90355814, 0.81277492, 0.27582141, 0.34052255, 0.09211961,
                          0.38139711, 0.40588835, 0.72562255, 0.46843548, 0.53552493,
                          0.10983976, 0.70222301, 0.33678326, 0.5755877 , 0.25931049,
                          0.4508404 , 0.61309158, 0.93828205, 0.54132972, 0.74656135,
                          0.66254458, 0.79255496, 0.15105507, 0.73739719, 0.8699253 ,
                          0.30037259, 0.18970023, 0.7351555 , 0.19264707, 0.66209567,
                          0.96806941, 0.64975045, 0.54480589, 0.11455497, 0.48495469,
                          0.76967885, 0.23472007, 0.43837476, 0.95659971, 0.66906068,
                          0.06233808, 0.89998454, 0.61301419, 0.83415149, 0.08989232,
                          0.68242323, 0.5311455 , 0.04401578, 0.64268437, 0.17462098,
                          0.91385774, 0.3254543 , 0.05379172, 0.77842952, 0.53732987,
                          0.83781747, 0.7615337 , 0.62005118, 0.73293008, 0.82346972,
                          0.2179367 , 0.36317936, 0.55619014, 0.09312705, 0.4694652 ,
                          0.69716073, 0.02440034, 0.53107043, 0.97073677, 0.87477045,
                          0.68304994, 0.78714746, 0.23263201, 0.42970537, 0.72939955,
                          0.12454156, 0.63073136, 0.31116734, 0.39634253, 0.1281059 ,
                          0.23014438, 0.13811186, 0.23720232, 0.16020445, 0.10732273,
                          0.90941219, 0.89549355, 0.71971555, 0.01863396, 0.63512323,
                          0.57834974, 0.74377666, 0.67809585, 0.69763216, 0.75809662,
                          0.34523363, 0.42721884, 0.06343445, 0.4707797 , 0.91267953,
                          0.9522137 , 0.429135  , 0.94506974, 0.42002389, 0.48045774,
                          0.1877623 , 0.93541748, 0.10358528, 0.90585229, 0.94482345,
                          0.85486823, 0.71201071, 0.82729667, 0.67685002, 0.89074951,
                          0.13603059, 0.29549921, 0.5829021 , 0.85710379, 0.33597495,
                          0.2635317 , 0.54822056, 0.13518258, 0.07510343, 0.57277576,
                          0.66026008, 0.10590873, 0.40988651, 0.73181046, 0.21849923,
                          0.68193615, 0.4861005 , 0.90062638, 0.49503759, 0.53109181,
                          0.31197913, 0.8260051 , 0.56845431, 0.20510746, 0.48927707,
                          0.84796951, 0.57021869, 0.32869802, 0.00649644, 0.89085066,
                          0.58793337, 0.13725509, 0.49166181, 0.79467837, 0.55550476,
                          0.86168924, 0.26284446, 0.34931772, 0.69039842, 0.04226658,
                          0.91252659, 0.8532767 , 0.15745086, 0.11244899, 0.35188228,
                          0.66119509, 0.88971845, 0.90199259, 0.53564388, 0.08103036,
                          0.89537074, 0.43988547, 0.39234971, 0.90744335, 0.87819375,
                          0.25940274, 0.48165619, 0.08404158, 0.16900508, 0.20502448,
                          0.00336955, 0.94376888, 0.89722214, 0.06817336, 0.35272289,
                          0.34452052, 0.23363246, 0.79650105, 0.8107239 , 0.94490429,
                          0.26741852, 0.87105166, 0.25525768, 0.26586211, 0.6449152 ,
                          0.10839033, 0.6871309 , 0.59008043, 0.07558712, 0.99527881,
                          0.13052048, 0.81075174, 0.38967993, 0.25408067, 0.78035165,
                          0.48123955, 0.97775619, 0.50408867, 0.51411035, 0.17947261,
                          0.99740746, 0.84538866, 0.62373254, 0.38782162, 0.55585207,
                          0.24743969, 0.25980163, 0.50272755, 0.76170535, 0.338618  ,
                          0.33580793, 0.58798537, 0.13328799, 0.01026525, 0.13839967,
                          0.31984267, 0.72693472, 0.84737434, 0.97859975, 0.61637914,
                          0.23018791, 0.89805651, 0.69772024, 0.7491404 , 0.3818782 ,
                          0.50000777, 0.71398283, 0.29910862, 0.36270529, 0.93178041,
                          0.03156497, 0.57674924, 0.13573152, 0.59758916, 0.47467419,
                          0.21707829, 0.36305461, 0.58480959, 0.18659161, 0.20999611,
                          0.23732489, 0.11099326, 0.49055309, 0.52547794, 0.01722654,
                          0.19637688, 0.03560497, 0.89843994, 0.34941756, 0.10796044,
                          0.07166564, 0.67297414, 0.34139877, 0.56321003, 0.13224438,
                          0.58789568, 0.05265614, 0.93254668, 0.41326988, 0.67692951,
                          0.27922074, 0.38788297, 0.24478052, 0.29147   , 0.80741949,
                          0.67936156, 0.7442339 , 0.00343505, 0.97756934, 0.02268554,
                          0.56302353, 0.10718293, 0.11474464, 0.10465633, 0.04846449,
                          0.33695467, 0.43787266, 0.15092164, 0.80017919, 0.40017523,
                          0.40391072, 0.65025117, 0.3018835 , 0.15825793, 0.02963411,
                          0.85526189, 0.29678796, 0.23667277, 0.4013067 , 0.76988912,
                          0.06110263, 0.66297631, 0.79827956, 0.70776264, 0.07467447,
                          0.89814767, 0.00308201, 0.7823472 , 0.38646676, 0.75957091,
                          0.47684411, 0.7398732 , 0.09206989, 0.02529722, 0.1859329 ,
                          0.8380139 , 0.33920514, 0.0689636 , 0.07697459, 0.88068415,
                          0.1229827 , 0.89652486, 0.13968141, 0.38800301, 0.7728669 ,
                          0.16905682, 0.47354036, 0.90279152, 0.62008568, 0.82696116,
                          0.43869547, 0.94940345, 0.12938034, 0.20523408, 0.11727954,
                          0.54592836, 0.82115919, 0.96255349, 0.04999854, 0.15256932,
                          0.62537849, 0.15516518, 0.25683723, 0.85702076, 0.7925628 ,
                          0.46399241, 0.10106241, 0.73089281, 0.46200846, 0.24160109,
                          0.01349364, 0.94349643, 0.70886053, 0.06715038, 0.95042753,
                          0.38413263, 0.61285658, 0.97690412, 0.07900655, 0.21037925,
                          0.03351281, 0.36733596, 0.05601802, 0.50752553, 0.62088055,
                          0.94638543, 0.31649186, 0.19788369, 0.59813263, 0.31156879,
                          0.84129622, 0.18756002, 0.80252603, 0.44583102, 0.08424927,
                          0.8055779 , 0.89467745, 0.32244817, 0.5244238 , 0.38246742,
                          0.17552342, 0.09374914, 0.02755403, 0.86455687, 0.45570292,
                          0.58901182, 0.11888058, 0.65051228, 0.9634849 , 0.72370701,
                          0.6882061 , 0.30785926, 0.13060746, 0.29416438, 0.87322017,
                          0.26415365, 0.41275749, 0.44246432, 0.53266346, 0.0943344 ,
                          0.30480514, 0.37707017, 0.41691054, 0.94780656, 0.48190006,
                          0.55313378, 0.34750865, 0.4482111 , 0.62723585, 0.72810975,
                          0.05039657, 0.27579548, 0.03891394, 0.25236345, 0.53330415,
                          0.73508523, 0.07895291, 0.17533722, 0.35439847, 0.35161594,
                          0.56198773, 0.09715776, 0.65962064, 0.93017981, 0.69473252,
                          0.36962137, 0.01256946, 0.76358204, 0.71278475, 0.90981899,
                          0.12157694, 0.72489524, 0.35040793, 0.67236183, 0.70497179,
                          0.65668193, 0.42266533, 0.45514152, 0.13569985, 0.70177277,
                          0.47524764, 0.61467852, 0.7338517 , 0.54271988, 0.0514032 ,
                          0.75535566, 0.65658309, 0.01198156, 0.99577374, 0.22789359,
                          0.54174149, 0.25282717, 0.08002466, 0.26660684, 0.07484457,
                          0.87263561, 0.39011271, 0.93760461, 0.80665246, 0.38182704,
                          0.97697037, 0.89624951, 0.83816689, 0.39032672, 0.68852691,
                          0.85427299, 0.66773948, 0.9883756 , 0.06231242, 0.87223773,
                          0.12315628, 0.6709966 , 0.69840404, 0.19659599, 0.89613321,
                          0.79136648, 0.223493  , 0.8223016 , 0.81438973, 0.89321441,
                          0.82446078, 0.29317171, 0.23707863, 0.61798678, 0.85119219,
                          0.48374624, 0.18401164, 0.91021346, 0.18338856, 0.33581014,
                          0.51203974, 0.27373114, 0.26494583, 0.3909069 , 0.69500561,
                          0.44669268, 0.57792494, 0.92083106, 0.3760086 , 0.92086847,
                          0.98019746, 0.00930665, 0.91236066, 0.10589444, 0.98849831,
                          0.9937718 , 0.88560407, 0.20240646, 0.53980435, 0.21197806,
                          0.73511026, 0.91531094, 0.24419261, 0.79892127, 0.76623351,
                          0.32237578, 0.74371048, 0.89283081, 0.99471695, 0.59218713,
                          0.14229807, 0.26866107, 0.61418273, 0.53238885, 0.2847934 ,
                          0.33879263, 0.15419413, 0.81211755, 0.55982182, 0.33033445,
                          0.98925566, 0.21407401, 0.75933437, 0.50981508, 0.84659468,
                          0.27123332, 0.30602554, 0.78974943, 0.15961765, 0.75269879,
                          0.88404004, 0.25359787, 0.67575388, 0.10753205, 0.52492257,
                          0.2276367 , 0.57348205, 0.55631533, 0.48828726, 0.80950892,
                          0.68959411, 0.06038109, 0.3730253 , 0.44658293, 0.12323353,
                          0.90588169, 0.13484593, 0.58743073, 0.60592698, 0.67315081,
                          0.59887062, 0.3524358 , 0.47446065, 0.98078295, 0.31889862,
                          0.88225427, 0.81911728, 0.53942069, 0.6742203 , 0.73162166,
                          0.34597118, 0.70844054, 0.25029322, 0.29910746, 0.35906746,
                          0.53989701, 0.36776386, 0.04466711, 0.09399784, 0.53547235,
                          0.64757585, 0.03797524, 0.66378485, 0.34908186, 0.79601534,
                          0.10335962, 0.30468185, 0.57992791, 0.97139889, 0.40799129,
                          0.72985792, 0.65705408, 0.48913045, 0.46000752, 0.99260624,
                          0.52711571, 0.9383317 , 0.87126459, 0.16266698, 0.17428769,
                          0.11933665, 0.15703581, 0.17907467, 0.32411207, 0.56666047,
                          0.80868794, 0.49001672, 0.77590492, 0.63564239, 0.92169564,
                          0.5098178 , 0.40486284, 0.42978249, 0.4228141 , 0.33155423,
                          0.92695095, 0.40583767, 0.6635788 , 0.93854154, 0.48653097,
                          0.9305096 , 0.96009097, 0.03602708, 0.16874548, 0.4733966 ,
                          0.42363966, 0.18261131, 0.42653311, 0.48740795, 0.40008523,
                          0.35099519, 0.9641026 , 0.93447868, 0.16069199, 0.63925304,
                          0.17770585, 0.53600886, 0.72037108, 0.8454436 , 0.182311  ,
                          0.97860041, 0.41959913, 0.45109368, 0.24313804, 0.17884554,
                          0.04705525, 0.83247529, 0.89877392, 0.57362423, 0.27708354,
                          0.93649503, 0.43493419, 0.5422893 , 0.85565473, 0.86814896,
                          0.75788182, 0.02102082, 0.62643473, 0.24955471, 0.12775553,
                          0.96546452, 0.11001835, 0.82845919, 0.84811548, 0.21516077,
                          0.88871084, 0.55331041, 0.99744447, 0.70181741, 0.09492537,
                          0.18130881, 0.45487527, 0.82986703, 0.31207231, 0.08682494,
                          0.90971212, 0.40231716, 0.95428082, 0.10105085, 0.7243062 ,
                          0.87386255, 0.28549753, 0.90084605, 0.91034781, 0.44687279,
                          0.99318239, 0.58953929, 0.73074143, 0.05055378 };

    variableVector answer = { -6.34180841, -8.44235442, -7.66602685, -6.62791667, -6.30156652,
                              -7.81093903, -9.08319118, -7.62283755, -8.7120047 , -7.96533995,
                              -7.29110914, -7.63480242, -6.0360827 , -6.66816385, -6.38308499,
                              -8.06776472, -7.29777722, -7.77952498, -7.6470537 , -9.94159411,
                              -7.65257834, -5.90193479, -6.5591572 , -8.12839975, -8.56024681,
                              -7.40823637, -8.875604 };

    variableVector result;

    errorOut error = micromorphicLinearElasticity::computeReferenceHigherOrderStress( Gamma, C, result );

    if ( error ){
        error->print();
        results << "test_computeReferenceHigherOrderStress & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( result, answer ) ){
        results << "test_computeReferenceHigherOrderStress (test 1) & False\n";
        return 1;
    }

    //Test the Jacobian

    variableVector resultJ;
    variableMatrix dMdGamma;
    error = micromorphicLinearElasticity::computeReferenceHigherOrderStress( Gamma, C, resultJ, dMdGamma );

    if ( error ){
        error->print();
        results << "test_computeReferenceHigherOrderStress & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJ, answer ) ){
        results << "test_computeReferenceHigherOrderStress (test 2) & False\n";
        return 1;
    }

    //Test dMdGamma
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < Gamma.size(); i++ ){
        constantVector delta( Gamma.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeReferenceHigherOrderStress( Gamma + delta, C, result_P );

        if ( error ){
            error->print();
            results << "test_computeReferenceHigherOrderStress & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeReferenceHigherOrderStress( Gamma - delta, C, result_M );

        if ( error ){
            error->print();
            results << "test_computeReferenceHigherOrderStress & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMdGamma[j][i] ) ){
                results << "test_computeReferenceHigherOrderStress (test 3) & False\n";
                return 1;
            }
        }
    }

    results << "test_computeReferenceHigherOrderStress & True\n";
    return 0;
}

int test_computeLinearElasticTerm3( std::ofstream &results ){
    /*!
     * Test the computation of the third term for micromorphic linear elasticity.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector invCGamma = { 3.50845589, -1.63297374,  1.45320064,  5.96175258,  3.87979583,
                                 1.87176717, -0.9248906 ,  2.62094337,  1.33218361,  0.32076702,
                                 1.4736958 ,  1.12865617, -0.64009695,  0.59763605,  1.30927157,
                                 1.3281096 , -0.5869566 ,  0.44018241,  0.57800689, -0.28019742,
                                 0.10974229,  0.84147417,  0.33054528,  0.22469373, -0.15308855,
                                 0.50800348,  0.14458868 }; 

    variableVector referenceHigherOrderStress = { -6.34180841, -8.44235442, -7.66602685, -6.62791667, -6.30156652,
                                                  -7.81093903, -9.08319118, -7.62283755, -8.7120047 , -7.96533995,
                                                  -7.29110914, -7.63480242, -6.0360827 , -6.66816385, -6.38308499,
                                                  -8.06776472, -7.29777722, -7.77952498, -7.6470537 , -9.94159411,
                                                  -7.65257834, -5.90193479, -6.5591572 , -8.12839975, -8.56024681,
                                                  -7.40823637, -8.875604 };

    variableVector answer = { -121.37119446,  -44.30225806,  -15.29816588,
                              -122.96816543,  -40.25141813,  -15.71355416,
                              -120.88746809,  -47.45291642,  -15.17132276 };

    variableVector result;

    errorOut error = micromorphicLinearElasticity::computeLinearElasticTerm3( invCGamma, referenceHigherOrderStress, 
                                                                              result );

    if ( error ){
        error->print();
        results << "test_computeLinearElasticTerm3 & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( result, answer ) ){
        results << "test_computeLinearElasticTerm3 (test 1) & False\n";
        return 1;
    }

    //Test the Jacobians

    variableVector resultJ;
    variableMatrix dTerm3dInvCGamma, dTerm3dM;

    error = micromorphicLinearElasticity::computeLinearElasticTerm3( invCGamma, referenceHigherOrderStress,
                                                                     resultJ, dTerm3dInvCGamma, dTerm3dM );

    if ( error ){
        error->print();
        results << "test_computeLinearElasticTerm3 & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJ, answer ) ){
        results << "test_computeLinearElasticTerm3 (test 2) & False\n";
        return 1;
    }

    //Test dTerm3dInvCGamma
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < invCGamma.size(); i++ ){
        constantVector delta( invCGamma.size(), 0 );
        delta[i] = eps * fabs( invCGamma[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeLinearElasticTerm3( invCGamma + delta, referenceHigherOrderStress,
                                                                         result_P );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm3 & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeLinearElasticTerm3( invCGamma - delta, referenceHigherOrderStress,
                                                                         result_M );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm3 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm3dInvCGamma[j][i] ) ){
                results << "test_computeLinearElasticTerm3 (test 3) & False\n";
                return 1;
            }
        }
    }

    //Test dTerm3dM
    for ( unsigned int i = 0; i < referenceHigherOrderStress.size(); i++ ){
        constantVector delta( referenceHigherOrderStress.size(), 0 );
        delta[i] = eps * fabs( referenceHigherOrderStress[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeLinearElasticTerm3( invCGamma, referenceHigherOrderStress + delta,
                                                                         result_P );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm3 & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeLinearElasticTerm3( invCGamma, referenceHigherOrderStress - delta,
                                                                         result_M );

        if ( error ){
            error->print();
            results << "test_computeLinearElasticTerm3 & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dTerm3dM[j][i] ) ){
                results << "test_computeLinearElasticTerm3 (test 4) & False\n";
                return 1;
            }
        }
    }
    
    results << "test_computeLinearElasticTerm3 & True\n";
    return 0;
}

int test_linearElasticityReferenceDerivedMeasures( std::ofstream &results ){
    /*!
     * Test the micromorphic linear elastic constitutive model from the 
     * derived deformation measures.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector rightCauchyGreen = { 0.34852835, 0.47540122, 1.11252634,
                                        0.47540122, 1.49184663, 1.57435946,
                                        1.11252634, 1.57435946, 3.68235756 };

    variableVector Psi = { -0.02303102, -0.41101265, -0.36040573,
                           -0.14715403, -0.18045474, -0.8814645 ,
                           -0.36637526, -1.08887072, -1.44707636 };

    variableVector Gamma = { -0.31120922, -0.3563267 , -0.36573233, -0.0771914 , -0.24252804,
                             -0.4738459 , -0.35937075, -0.01781817, -0.17465609, -0.82225557,
                             -0.36719542, -0.86494826, -0.92750732, -1.18214541, -1.00423785,
                             -0.43125133, -0.19543115, -0.49736256, -0.67098335, -0.98433811,
                             -1.0183107 , -0.12645195, -0.79818076, -1.23318541, -0.95577138,
                              0.1274431 , -0.47648617 };

    parameterVector A = { 0.91738548, 0.5223949 , 0.04303308, 0.42138619, 0.71420722,
                          0.15443589, 0.05041408, 0.69624665, 0.8766614 , 0.17261697,
                          0.02350474, 0.59406857, 0.79573586, 0.21213138, 0.47821637,
                          0.35462425, 0.6291708 , 0.48565385, 0.67132896, 0.27926608,
                          0.04579313, 0.88556864, 0.08992741, 0.75316186, 0.76279627,
                          0.5635193 , 0.18529158, 0.05722408, 0.65275234, 0.97189144,
                          0.68435   , 0.96624106, 0.84374092, 0.21040392, 0.13887068,
                          0.34423717, 0.50801461, 0.28726825, 0.52590869, 0.36090934,
                          0.97602275, 0.95087184, 0.32716562, 0.50074403, 0.3955299 ,
                          0.48018626, 0.71150853, 0.61899609, 0.57103915, 0.5154469 ,
                          0.4456661 , 0.41192121, 0.12649935, 0.69069678, 0.17931527,
                          0.98427378, 0.82378172, 0.44572395, 0.90859147, 0.33180413,
                          0.30215348, 0.42929583, 0.61595281, 0.66534843, 0.31552903,
                          0.99326382, 0.87708958, 0.27827411, 0.30275486, 0.3209769 ,
                          0.81059907, 0.83577572, 0.54758756, 0.30482114, 0.00502004,
                          0.09242907, 0.24196602, 0.00779042, 0.04284832, 0.56224798,
                          0.86563423 };

    parameterVector B = { 0.99402085, 0.66339725, 0.73962847, 0.75991152, 0.91213988,
                          0.94984965, 0.6011931 , 0.32834193, 0.21231827, 0.65420996,
                          0.66533091, 0.83293238, 0.2537865 , 0.57946922, 0.79358565,
                          0.92885037, 0.0923514 , 0.12441041, 0.87678012, 0.87730359,
                          0.59116472, 0.21901437, 0.45976152, 0.86524067, 0.06668473,
                          0.83812813, 0.06951684, 0.91636315, 0.07878975, 0.3887551 ,
                          0.86632579, 0.84909984, 0.72558761, 0.12280263, 0.92078995,
                          0.48305302, 0.19393044, 0.82984994, 0.27981095, 0.60669024,
                          0.25483571, 0.2663953 , 0.3504269 , 0.50945399, 0.15647713,
                          0.46803744, 0.44503108, 0.86396469, 0.44057713, 0.97430525,
                          0.12593544, 0.48379355, 0.82188636, 0.31297267, 0.21986847,
                          0.8032502 , 0.07287772, 0.6974731 , 0.97608146, 0.49354539,
                          0.97886936, 0.16204678, 0.79205693, 0.81275365, 0.44670719,
                          0.22577849, 0.14381145, 0.92846116, 0.13905519, 0.36151037,
                          0.71576657, 0.5462745 , 0.41204395, 0.79415741, 0.73346637,
                          0.51829857, 0.31806782, 0.73860407, 0.21137896, 0.06281619,
                          0.08517056 };

    parameterVector C = { 0.73168423, 0.20787282, 0.44597068, 0.13971472, 0.67623962,
                          0.01024113, 0.26235898, 0.52638878, 0.55822266, 0.79169357,
                          0.79556295, 0.26274848, 0.22158735, 0.77447856, 0.09905053,
                          0.24537506, 0.82422833, 0.13912553, 0.13704714, 0.60418098,
                          0.97916951, 0.96975567, 0.15156735, 0.33820056, 0.40984014,
                          0.03316931, 0.07805217, 0.09086063, 0.0587449 , 0.93973718,
                          0.2088402 , 0.27030923, 0.90893679, 0.45495913, 0.38558114,
                          0.89599555, 0.99117823, 0.68663521, 0.47807759, 0.47462775,
                          0.63545614, 0.76359775, 0.58543208, 0.67889697, 0.23400169,
                          0.90355814, 0.81277492, 0.27582141, 0.34052255, 0.09211961,
                          0.38139711, 0.40588835, 0.72562255, 0.46843548, 0.53552493,
                          0.10983976, 0.70222301, 0.33678326, 0.5755877 , 0.25931049,
                          0.4508404 , 0.61309158, 0.93828205, 0.54132972, 0.74656135,
                          0.66254458, 0.79255496, 0.15105507, 0.73739719, 0.8699253 ,
                          0.30037259, 0.18970023, 0.7351555 , 0.19264707, 0.66209567,
                          0.96806941, 0.64975045, 0.54480589, 0.11455497, 0.48495469,
                          0.76967885, 0.23472007, 0.43837476, 0.95659971, 0.66906068,
                          0.06233808, 0.89998454, 0.61301419, 0.83415149, 0.08989232,
                          0.68242323, 0.5311455 , 0.04401578, 0.64268437, 0.17462098,
                          0.91385774, 0.3254543 , 0.05379172, 0.77842952, 0.53732987,
                          0.83781747, 0.7615337 , 0.62005118, 0.73293008, 0.82346972,
                          0.2179367 , 0.36317936, 0.55619014, 0.09312705, 0.4694652 ,
                          0.69716073, 0.02440034, 0.53107043, 0.97073677, 0.87477045,
                          0.68304994, 0.78714746, 0.23263201, 0.42970537, 0.72939955,
                          0.12454156, 0.63073136, 0.31116734, 0.39634253, 0.1281059 ,
                          0.23014438, 0.13811186, 0.23720232, 0.16020445, 0.10732273,
                          0.90941219, 0.89549355, 0.71971555, 0.01863396, 0.63512323,
                          0.57834974, 0.74377666, 0.67809585, 0.69763216, 0.75809662,
                          0.34523363, 0.42721884, 0.06343445, 0.4707797 , 0.91267953,
                          0.9522137 , 0.429135  , 0.94506974, 0.42002389, 0.48045774,
                          0.1877623 , 0.93541748, 0.10358528, 0.90585229, 0.94482345,
                          0.85486823, 0.71201071, 0.82729667, 0.67685002, 0.89074951,
                          0.13603059, 0.29549921, 0.5829021 , 0.85710379, 0.33597495,
                          0.2635317 , 0.54822056, 0.13518258, 0.07510343, 0.57277576,
                          0.66026008, 0.10590873, 0.40988651, 0.73181046, 0.21849923,
                          0.68193615, 0.4861005 , 0.90062638, 0.49503759, 0.53109181,
                          0.31197913, 0.8260051 , 0.56845431, 0.20510746, 0.48927707,
                          0.84796951, 0.57021869, 0.32869802, 0.00649644, 0.89085066,
                          0.58793337, 0.13725509, 0.49166181, 0.79467837, 0.55550476,
                          0.86168924, 0.26284446, 0.34931772, 0.69039842, 0.04226658,
                          0.91252659, 0.8532767 , 0.15745086, 0.11244899, 0.35188228,
                          0.66119509, 0.88971845, 0.90199259, 0.53564388, 0.08103036,
                          0.89537074, 0.43988547, 0.39234971, 0.90744335, 0.87819375,
                          0.25940274, 0.48165619, 0.08404158, 0.16900508, 0.20502448,
                          0.00336955, 0.94376888, 0.89722214, 0.06817336, 0.35272289,
                          0.34452052, 0.23363246, 0.79650105, 0.8107239 , 0.94490429,
                          0.26741852, 0.87105166, 0.25525768, 0.26586211, 0.6449152 ,
                          0.10839033, 0.6871309 , 0.59008043, 0.07558712, 0.99527881,
                          0.13052048, 0.81075174, 0.38967993, 0.25408067, 0.78035165,
                          0.48123955, 0.97775619, 0.50408867, 0.51411035, 0.17947261,
                          0.99740746, 0.84538866, 0.62373254, 0.38782162, 0.55585207,
                          0.24743969, 0.25980163, 0.50272755, 0.76170535, 0.338618  ,
                          0.33580793, 0.58798537, 0.13328799, 0.01026525, 0.13839967,
                          0.31984267, 0.72693472, 0.84737434, 0.97859975, 0.61637914,
                          0.23018791, 0.89805651, 0.69772024, 0.7491404 , 0.3818782 ,
                          0.50000777, 0.71398283, 0.29910862, 0.36270529, 0.93178041,
                          0.03156497, 0.57674924, 0.13573152, 0.59758916, 0.47467419,
                          0.21707829, 0.36305461, 0.58480959, 0.18659161, 0.20999611,
                          0.23732489, 0.11099326, 0.49055309, 0.52547794, 0.01722654,
                          0.19637688, 0.03560497, 0.89843994, 0.34941756, 0.10796044,
                          0.07166564, 0.67297414, 0.34139877, 0.56321003, 0.13224438,
                          0.58789568, 0.05265614, 0.93254668, 0.41326988, 0.67692951,
                          0.27922074, 0.38788297, 0.24478052, 0.29147   , 0.80741949,
                          0.67936156, 0.7442339 , 0.00343505, 0.97756934, 0.02268554,
                          0.56302353, 0.10718293, 0.11474464, 0.10465633, 0.04846449,
                          0.33695467, 0.43787266, 0.15092164, 0.80017919, 0.40017523,
                          0.40391072, 0.65025117, 0.3018835 , 0.15825793, 0.02963411,
                          0.85526189, 0.29678796, 0.23667277, 0.4013067 , 0.76988912,
                          0.06110263, 0.66297631, 0.79827956, 0.70776264, 0.07467447,
                          0.89814767, 0.00308201, 0.7823472 , 0.38646676, 0.75957091,
                          0.47684411, 0.7398732 , 0.09206989, 0.02529722, 0.1859329 ,
                          0.8380139 , 0.33920514, 0.0689636 , 0.07697459, 0.88068415,
                          0.1229827 , 0.89652486, 0.13968141, 0.38800301, 0.7728669 ,
                          0.16905682, 0.47354036, 0.90279152, 0.62008568, 0.82696116,
                          0.43869547, 0.94940345, 0.12938034, 0.20523408, 0.11727954,
                          0.54592836, 0.82115919, 0.96255349, 0.04999854, 0.15256932,
                          0.62537849, 0.15516518, 0.25683723, 0.85702076, 0.7925628 ,
                          0.46399241, 0.10106241, 0.73089281, 0.46200846, 0.24160109,
                          0.01349364, 0.94349643, 0.70886053, 0.06715038, 0.95042753,
                          0.38413263, 0.61285658, 0.97690412, 0.07900655, 0.21037925,
                          0.03351281, 0.36733596, 0.05601802, 0.50752553, 0.62088055,
                          0.94638543, 0.31649186, 0.19788369, 0.59813263, 0.31156879,
                          0.84129622, 0.18756002, 0.80252603, 0.44583102, 0.08424927,
                          0.8055779 , 0.89467745, 0.32244817, 0.5244238 , 0.38246742,
                          0.17552342, 0.09374914, 0.02755403, 0.86455687, 0.45570292,
                          0.58901182, 0.11888058, 0.65051228, 0.9634849 , 0.72370701,
                          0.6882061 , 0.30785926, 0.13060746, 0.29416438, 0.87322017,
                          0.26415365, 0.41275749, 0.44246432, 0.53266346, 0.0943344 ,
                          0.30480514, 0.37707017, 0.41691054, 0.94780656, 0.48190006,
                          0.55313378, 0.34750865, 0.4482111 , 0.62723585, 0.72810975,
                          0.05039657, 0.27579548, 0.03891394, 0.25236345, 0.53330415,
                          0.73508523, 0.07895291, 0.17533722, 0.35439847, 0.35161594,
                          0.56198773, 0.09715776, 0.65962064, 0.93017981, 0.69473252,
                          0.36962137, 0.01256946, 0.76358204, 0.71278475, 0.90981899,
                          0.12157694, 0.72489524, 0.35040793, 0.67236183, 0.70497179,
                          0.65668193, 0.42266533, 0.45514152, 0.13569985, 0.70177277,
                          0.47524764, 0.61467852, 0.7338517 , 0.54271988, 0.0514032 ,
                          0.75535566, 0.65658309, 0.01198156, 0.99577374, 0.22789359,
                          0.54174149, 0.25282717, 0.08002466, 0.26660684, 0.07484457,
                          0.87263561, 0.39011271, 0.93760461, 0.80665246, 0.38182704,
                          0.97697037, 0.89624951, 0.83816689, 0.39032672, 0.68852691,
                          0.85427299, 0.66773948, 0.9883756 , 0.06231242, 0.87223773,
                          0.12315628, 0.6709966 , 0.69840404, 0.19659599, 0.89613321,
                          0.79136648, 0.223493  , 0.8223016 , 0.81438973, 0.89321441,
                          0.82446078, 0.29317171, 0.23707863, 0.61798678, 0.85119219,
                          0.48374624, 0.18401164, 0.91021346, 0.18338856, 0.33581014,
                          0.51203974, 0.27373114, 0.26494583, 0.3909069 , 0.69500561,
                          0.44669268, 0.57792494, 0.92083106, 0.3760086 , 0.92086847,
                          0.98019746, 0.00930665, 0.91236066, 0.10589444, 0.98849831,
                          0.9937718 , 0.88560407, 0.20240646, 0.53980435, 0.21197806,
                          0.73511026, 0.91531094, 0.24419261, 0.79892127, 0.76623351,
                          0.32237578, 0.74371048, 0.89283081, 0.99471695, 0.59218713,
                          0.14229807, 0.26866107, 0.61418273, 0.53238885, 0.2847934 ,
                          0.33879263, 0.15419413, 0.81211755, 0.55982182, 0.33033445,
                          0.98925566, 0.21407401, 0.75933437, 0.50981508, 0.84659468,
                          0.27123332, 0.30602554, 0.78974943, 0.15961765, 0.75269879,
                          0.88404004, 0.25359787, 0.67575388, 0.10753205, 0.52492257,
                          0.2276367 , 0.57348205, 0.55631533, 0.48828726, 0.80950892,
                          0.68959411, 0.06038109, 0.3730253 , 0.44658293, 0.12323353,
                          0.90588169, 0.13484593, 0.58743073, 0.60592698, 0.67315081,
                          0.59887062, 0.3524358 , 0.47446065, 0.98078295, 0.31889862,
                          0.88225427, 0.81911728, 0.53942069, 0.6742203 , 0.73162166,
                          0.34597118, 0.70844054, 0.25029322, 0.29910746, 0.35906746,
                          0.53989701, 0.36776386, 0.04466711, 0.09399784, 0.53547235,
                          0.64757585, 0.03797524, 0.66378485, 0.34908186, 0.79601534,
                          0.10335962, 0.30468185, 0.57992791, 0.97139889, 0.40799129,
                          0.72985792, 0.65705408, 0.48913045, 0.46000752, 0.99260624,
                          0.52711571, 0.9383317 , 0.87126459, 0.16266698, 0.17428769,
                          0.11933665, 0.15703581, 0.17907467, 0.32411207, 0.56666047,
                          0.80868794, 0.49001672, 0.77590492, 0.63564239, 0.92169564,
                          0.5098178 , 0.40486284, 0.42978249, 0.4228141 , 0.33155423,
                          0.92695095, 0.40583767, 0.6635788 , 0.93854154, 0.48653097,
                          0.9305096 , 0.96009097, 0.03602708, 0.16874548, 0.4733966 ,
                          0.42363966, 0.18261131, 0.42653311, 0.48740795, 0.40008523,
                          0.35099519, 0.9641026 , 0.93447868, 0.16069199, 0.63925304,
                          0.17770585, 0.53600886, 0.72037108, 0.8454436 , 0.182311  ,
                          0.97860041, 0.41959913, 0.45109368, 0.24313804, 0.17884554,
                          0.04705525, 0.83247529, 0.89877392, 0.57362423, 0.27708354,
                          0.93649503, 0.43493419, 0.5422893 , 0.85565473, 0.86814896,
                          0.75788182, 0.02102082, 0.62643473, 0.24955471, 0.12775553,
                          0.96546452, 0.11001835, 0.82845919, 0.84811548, 0.21516077,
                          0.88871084, 0.55331041, 0.99744447, 0.70181741, 0.09492537,
                          0.18130881, 0.45487527, 0.82986703, 0.31207231, 0.08682494,
                          0.90971212, 0.40231716, 0.95428082, 0.10105085, 0.7243062 ,
                          0.87386255, 0.28549753, 0.90084605, 0.91034781, 0.44687279,
                          0.99318239, 0.58953929, 0.73074143, 0.05055378 };

    parameterVector D = { 0.146517  , 0.3707709 , 0.08328664, 0.49256865, 0.69405928,
                          0.61837293, 0.47376479, 0.11188706, 0.2228905 , 0.36340832,
                          0.0693854 , 0.10615699, 0.88785036, 0.6677967 , 0.63177135,
                          0.57125641, 0.80023305, 0.08298528, 0.91838322, 0.18315431,
                          0.89992512, 0.53899603, 0.41261589, 0.31495081, 0.83742576,
                          0.09300794, 0.82360698, 0.21493177, 0.1629844 , 0.21152065,
                          0.16961513, 0.4914438 , 0.50520605, 0.14553687, 0.26358359,
                          0.75964966, 0.65752746, 0.71537866, 0.0052241 , 0.96884752,
                          0.39184445, 0.9992278 , 0.82985355, 0.77985287, 0.82259158,
                          0.40209148, 0.65923576, 0.86734155, 0.82929213, 0.45634802,
                          0.27689889, 0.96708886, 0.1655354 , 0.89114231, 0.19536992,
                          0.33743959, 0.73300973, 0.363747  , 0.26554793, 0.61940794,
                          0.68798717, 0.31331865, 0.29169741, 0.94606427, 0.57961661,
                          0.9410199 , 0.09121453, 0.42521405, 0.67622819, 0.69165347,
                          0.84437122, 0.21535456, 0.15999728, 0.34674508, 0.86254606,
                          0.04035766, 0.19320383, 0.05731681, 0.95023339, 0.38534862,
                          0.01313969 };

    variableVector answerPK2Stress = { 314.71116295,   30.09663087,  -97.50775502,
                                       275.16187648,   28.28022151,  -85.63390628,
                                       345.9794607 ,   29.69720043, -103.26896348 };

    variableVector answerMicroStress = { 630.03379183,  306.44161512,  248.73797179,
                                         306.21515887,   59.80476285,  -54.44381993,
                                         251.26329613,  -53.54715409, -206.44832422 };

    variableVector answerHigherOrderStress = { -6.34180841, -8.44235442, -7.66602685, -6.62791667, -6.30156652,
                                               -7.81093903, -9.08319118, -7.62283755, -8.7120047 , -7.96533995,
                                               -7.29110914, -7.63480242, -6.0360827 , -6.66816385, -6.38308499,
                                               -8.06776472, -7.29777722, -7.77952498, -7.6470537 , -9.94159411,
                                               -7.65257834, -5.90193479, -6.5591572 , -8.12839975, -8.56024681,
                                               -7.40823637, -8.875604 };

    variableVector resultPK2Stress, resultMicroStress, resultHigherOrderStress;

    errorOut error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen, Psi, Gamma,
                                                                                             A, B, C, D,
                                                                                             resultPK2Stress, resultMicroStress,
                                                                                             resultHigherOrderStress );

    if ( error ){
        error->print();
        results << "test_linearElasticityReferenceDerivedMeasures & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultPK2Stress, answerPK2Stress ) ){
        results << "test_linearElasticityReferenceDerivedMeasures (test 1) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultMicroStress, answerMicroStress ) ){
        results << "test_linearElasticityReferenceDerivedMeasures (test 2) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultHigherOrderStress, answerHigherOrderStress ) ){
        results << "test_linearElasticityReferenceDerivedMeasures (test 3) & False\n";
        return 1;
    }

    variableVector resultPK2StressJ, resultMicroStressJ, resultHigherOrderStressJ;
    variableMatrix dPK2dRCG, dPK2dPsi, dPK2dGamma, dSigmadRCG, dSigmadPsi, dSigmadGamma, dMdGamma;

    //Test the Jacobians
    error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen, Psi, Gamma,
                                                                                    A, B, C, D,
                                                                                    resultPK2StressJ, resultMicroStressJ,
                                                                                    resultHigherOrderStressJ, dPK2dRCG,
                                                                                    dPK2dPsi, dPK2dGamma, dSigmadRCG,
                                                                                    dSigmadPsi, dSigmadGamma, dMdGamma );

    if ( error ){
        error->print();
        results << "test_linearElasticityReferenceDerivedMeasures & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultPK2StressJ, answerPK2Stress ) ){
        results << "test_linearElasticityReferenceDerivedMeasures (test 4) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultMicroStressJ, answerMicroStress ) ){
        results << "test_linearElasticityReferenceDerivedMeasures (test 5) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultHigherOrderStressJ, answerHigherOrderStress ) ){
        results << "test_linearElasticityReferenceDerivedMeasures (test 6) & False\n";
        return 1;
    }

    //Test the derivatives w.r.t. the right cauchy green deformation tensor
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < rightCauchyGreen.size(); i++ ){
        constantVector delta( rightCauchyGreen.size(), 0 );
        delta[i] = eps * fabs( rightCauchyGreen[i] ) + eps;

        variableVector PK2P, SigmaP, MP;
        variableVector PK2M, SigmaM, MM;

        error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen + delta, Psi, Gamma,
                                                                                        A, B, C, D,
                                                                                        PK2P, SigmaP, MP );

        if ( error ){
            error->print();
            results << "test_linearElasticityReferenceDerivedMeasures & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen - delta, Psi, Gamma,
                                                                                        A, B, C, D,
                                                                                        PK2M, SigmaM, MM );

        if ( error ){
            error->print();
            results << "test_linearElasticityReferenceDerivedMeasures & False\n";
            return 1;
        }

        variableVector gradCol = ( PK2P - PK2M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], dPK2dRCG[ j ][ i ] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 7) & False\n";
                return 1;
            }
        }

        gradCol = ( SigmaP - SigmaM ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], dSigmadRCG[ j ][ i ] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 8) & False\n";
                return 1;
            }
        }

        gradCol = ( MP - MM ) / ( 2 * delta[i] );

        if ( !vectorTools::fuzzyEquals( gradCol, variableVector( gradCol.size(), 0 ) ) ){
            results << "test_linearElasticityReferenceDerivedMeasures (test 9) & False\n";
            return 1;
        }
    }

    //Test the derivatives w.r.t. the micro deformation measure Psi
    for ( unsigned int i = 0; i < Psi.size(); i++ ){
        constantVector delta( Psi.size(), 0 );
        delta[i] = eps * fabs( Psi[i] ) + eps;

        variableVector PK2P, SigmaP, MP;
        variableVector PK2M, SigmaM, MM;

        error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen, Psi + delta, Gamma,
                                                                                        A, B, C, D,
                                                                                        PK2P, SigmaP, MP );

        if ( error ){
            error->print();
            results << "test_linearElasticityReferenceDerivedMeasures & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen, Psi - delta, Gamma,
                                                                                        A, B, C, D,
                                                                                        PK2M, SigmaM, MM );

        if ( error ){
            error->print();
            results << "test_linearElasticityReferenceDerivedMeasures & False\n";
            return 1;
        }

        variableVector gradCol = ( PK2P - PK2M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], dPK2dPsi[ j ][ i ] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 10) & False\n";
                return 1;
            }
        }

        gradCol = ( SigmaP - SigmaM ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], dSigmadPsi[ j ][ i ] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 11) & False\n";
                return 1;
            }
        }

        gradCol = ( MP - MM ) / ( 2 * delta[i] );

        if ( !vectorTools::fuzzyEquals( gradCol, variableVector( gradCol.size(), 0 ) ) ){
            results << "test_linearElasticityReferenceDerivedMeasures (test 12) & False\n";
            return 1;
        }
    }

    //Test the derivatives w.r.t. the higher order deformation measure Gamma
    for ( unsigned int i = 0; i < Gamma.size(); i++ ){
        constantVector delta( Gamma.size(), 0 );
        delta[i] = eps * fabs( Gamma[i] ) + eps;

        variableVector PK2P, SigmaP, MP;
        variableVector PK2M, SigmaM, MM;

        error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen, Psi, Gamma + delta,
                                                                                        A, B, C, D,
                                                                                        PK2P, SigmaP, MP );

        if ( error ){
            error->print();
            results << "test_linearElasticityReferenceDerivedMeasures & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::linearElasticityReferenceDerivedMeasures( rightCauchyGreen, Psi, Gamma - delta,
                                                                                        A, B, C, D,
                                                                                        PK2M, SigmaM, MM );

        if ( error ){
            error->print();
            results << "test_linearElasticityReferenceDerivedMeasures & False\n";
            return 1;
        }

        variableVector gradCol = ( PK2P - PK2M ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], dPK2dGamma[ j ][ i ] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 13) & False\n";
                return 1;
            }
        }

        gradCol = ( SigmaP - SigmaM ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], dSigmadGamma[ j ][ i ] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 14) & False\n";
                return 1;
            }
        }

        gradCol = ( MP - MM ) / ( 2 * delta[i] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMdGamma[j][i] ) ){
                results << "test_linearElasticityReferenceDerivedMeasures (test 15) & False\n";
                return 1;
            }
        }
    }

    results << "test_linearElasticityReferenceDerivedMeasures & True\n";
    return 0;
}

int test_linearElasticityReference( std::ofstream &results ){
    /*!
     * Test the micromorphic linear elastic constitutive model.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector deformationGradient = { -0.50668478, -0.48303615, -1.43907185,
                                            0.24106815,  0.10656166,  1.06851718,
                                           -0.18353482, -1.11676646, -0.68534721 };

    variableVector microDeformation = { -0.36302711,  0.94624033,  0.07877948,
                                        -0.68872716,  0.10276167, -0.81357538,
                                         0.22307023, -0.23788599,  0.67759487 };

    variableVector gradientMicroDeformation = { 0.80507219, 0.73460211, 0.66571977, 0.13571332, 0.18559912,
                                                0.99230253, 0.77887526, 0.23648914, 0.31711178, 0.75118698,
                                                0.08013972, 0.27232507, 0.59595994, 0.13892773, 0.51533812,
                                                0.19823639, 0.51598785, 0.19048906, 0.45974189, 0.01871104,
                                                0.51255207, 0.82869552, 0.99152216, 0.51920895, 0.06818867,
                                                0.12194391, 0.32637525 };

    parameterVector A = { 0.91738548, 0.5223949 , 0.04303308, 0.42138619, 0.71420722,
                          0.15443589, 0.05041408, 0.69624665, 0.8766614 , 0.17261697,
                          0.02350474, 0.59406857, 0.79573586, 0.21213138, 0.47821637,
                          0.35462425, 0.6291708 , 0.48565385, 0.67132896, 0.27926608,
                          0.04579313, 0.88556864, 0.08992741, 0.75316186, 0.76279627,
                          0.5635193 , 0.18529158, 0.05722408, 0.65275234, 0.97189144,
                          0.68435   , 0.96624106, 0.84374092, 0.21040392, 0.13887068,
                          0.34423717, 0.50801461, 0.28726825, 0.52590869, 0.36090934,
                          0.97602275, 0.95087184, 0.32716562, 0.50074403, 0.3955299 ,
                          0.48018626, 0.71150853, 0.61899609, 0.57103915, 0.5154469 ,
                          0.4456661 , 0.41192121, 0.12649935, 0.69069678, 0.17931527,
                          0.98427378, 0.82378172, 0.44572395, 0.90859147, 0.33180413,
                          0.30215348, 0.42929583, 0.61595281, 0.66534843, 0.31552903,
                          0.99326382, 0.87708958, 0.27827411, 0.30275486, 0.3209769 ,
                          0.81059907, 0.83577572, 0.54758756, 0.30482114, 0.00502004,
                          0.09242907, 0.24196602, 0.00779042, 0.04284832, 0.56224798,
                          0.86563423 };

    parameterVector B = { 0.99402085, 0.66339725, 0.73962847, 0.75991152, 0.91213988,
                          0.94984965, 0.6011931 , 0.32834193, 0.21231827, 0.65420996,
                          0.66533091, 0.83293238, 0.2537865 , 0.57946922, 0.79358565,
                          0.92885037, 0.0923514 , 0.12441041, 0.87678012, 0.87730359,
                          0.59116472, 0.21901437, 0.45976152, 0.86524067, 0.06668473,
                          0.83812813, 0.06951684, 0.91636315, 0.07878975, 0.3887551 ,
                          0.86632579, 0.84909984, 0.72558761, 0.12280263, 0.92078995,
                          0.48305302, 0.19393044, 0.82984994, 0.27981095, 0.60669024,
                          0.25483571, 0.2663953 , 0.3504269 , 0.50945399, 0.15647713,
                          0.46803744, 0.44503108, 0.86396469, 0.44057713, 0.97430525,
                          0.12593544, 0.48379355, 0.82188636, 0.31297267, 0.21986847,
                          0.8032502 , 0.07287772, 0.6974731 , 0.97608146, 0.49354539,
                          0.97886936, 0.16204678, 0.79205693, 0.81275365, 0.44670719,
                          0.22577849, 0.14381145, 0.92846116, 0.13905519, 0.36151037,
                          0.71576657, 0.5462745 , 0.41204395, 0.79415741, 0.73346637,
                          0.51829857, 0.31806782, 0.73860407, 0.21137896, 0.06281619,
                          0.08517056 };

    parameterVector C = { 0.73168423, 0.20787282, 0.44597068, 0.13971472, 0.67623962,
                          0.01024113, 0.26235898, 0.52638878, 0.55822266, 0.79169357,
                          0.79556295, 0.26274848, 0.22158735, 0.77447856, 0.09905053,
                          0.24537506, 0.82422833, 0.13912553, 0.13704714, 0.60418098,
                          0.97916951, 0.96975567, 0.15156735, 0.33820056, 0.40984014,
                          0.03316931, 0.07805217, 0.09086063, 0.0587449 , 0.93973718,
                          0.2088402 , 0.27030923, 0.90893679, 0.45495913, 0.38558114,
                          0.89599555, 0.99117823, 0.68663521, 0.47807759, 0.47462775,
                          0.63545614, 0.76359775, 0.58543208, 0.67889697, 0.23400169,
                          0.90355814, 0.81277492, 0.27582141, 0.34052255, 0.09211961,
                          0.38139711, 0.40588835, 0.72562255, 0.46843548, 0.53552493,
                          0.10983976, 0.70222301, 0.33678326, 0.5755877 , 0.25931049,
                          0.4508404 , 0.61309158, 0.93828205, 0.54132972, 0.74656135,
                          0.66254458, 0.79255496, 0.15105507, 0.73739719, 0.8699253 ,
                          0.30037259, 0.18970023, 0.7351555 , 0.19264707, 0.66209567,
                          0.96806941, 0.64975045, 0.54480589, 0.11455497, 0.48495469,
                          0.76967885, 0.23472007, 0.43837476, 0.95659971, 0.66906068,
                          0.06233808, 0.89998454, 0.61301419, 0.83415149, 0.08989232,
                          0.68242323, 0.5311455 , 0.04401578, 0.64268437, 0.17462098,
                          0.91385774, 0.3254543 , 0.05379172, 0.77842952, 0.53732987,
                          0.83781747, 0.7615337 , 0.62005118, 0.73293008, 0.82346972,
                          0.2179367 , 0.36317936, 0.55619014, 0.09312705, 0.4694652 ,
                          0.69716073, 0.02440034, 0.53107043, 0.97073677, 0.87477045,
                          0.68304994, 0.78714746, 0.23263201, 0.42970537, 0.72939955,
                          0.12454156, 0.63073136, 0.31116734, 0.39634253, 0.1281059 ,
                          0.23014438, 0.13811186, 0.23720232, 0.16020445, 0.10732273,
                          0.90941219, 0.89549355, 0.71971555, 0.01863396, 0.63512323,
                          0.57834974, 0.74377666, 0.67809585, 0.69763216, 0.75809662,
                          0.34523363, 0.42721884, 0.06343445, 0.4707797 , 0.91267953,
                          0.9522137 , 0.429135  , 0.94506974, 0.42002389, 0.48045774,
                          0.1877623 , 0.93541748, 0.10358528, 0.90585229, 0.94482345,
                          0.85486823, 0.71201071, 0.82729667, 0.67685002, 0.89074951,
                          0.13603059, 0.29549921, 0.5829021 , 0.85710379, 0.33597495,
                          0.2635317 , 0.54822056, 0.13518258, 0.07510343, 0.57277576,
                          0.66026008, 0.10590873, 0.40988651, 0.73181046, 0.21849923,
                          0.68193615, 0.4861005 , 0.90062638, 0.49503759, 0.53109181,
                          0.31197913, 0.8260051 , 0.56845431, 0.20510746, 0.48927707,
                          0.84796951, 0.57021869, 0.32869802, 0.00649644, 0.89085066,
                          0.58793337, 0.13725509, 0.49166181, 0.79467837, 0.55550476,
                          0.86168924, 0.26284446, 0.34931772, 0.69039842, 0.04226658,
                          0.91252659, 0.8532767 , 0.15745086, 0.11244899, 0.35188228,
                          0.66119509, 0.88971845, 0.90199259, 0.53564388, 0.08103036,
                          0.89537074, 0.43988547, 0.39234971, 0.90744335, 0.87819375,
                          0.25940274, 0.48165619, 0.08404158, 0.16900508, 0.20502448,
                          0.00336955, 0.94376888, 0.89722214, 0.06817336, 0.35272289,
                          0.34452052, 0.23363246, 0.79650105, 0.8107239 , 0.94490429,
                          0.26741852, 0.87105166, 0.25525768, 0.26586211, 0.6449152 ,
                          0.10839033, 0.6871309 , 0.59008043, 0.07558712, 0.99527881,
                          0.13052048, 0.81075174, 0.38967993, 0.25408067, 0.78035165,
                          0.48123955, 0.97775619, 0.50408867, 0.51411035, 0.17947261,
                          0.99740746, 0.84538866, 0.62373254, 0.38782162, 0.55585207,
                          0.24743969, 0.25980163, 0.50272755, 0.76170535, 0.338618  ,
                          0.33580793, 0.58798537, 0.13328799, 0.01026525, 0.13839967,
                          0.31984267, 0.72693472, 0.84737434, 0.97859975, 0.61637914,
                          0.23018791, 0.89805651, 0.69772024, 0.7491404 , 0.3818782 ,
                          0.50000777, 0.71398283, 0.29910862, 0.36270529, 0.93178041,
                          0.03156497, 0.57674924, 0.13573152, 0.59758916, 0.47467419,
                          0.21707829, 0.36305461, 0.58480959, 0.18659161, 0.20999611,
                          0.23732489, 0.11099326, 0.49055309, 0.52547794, 0.01722654,
                          0.19637688, 0.03560497, 0.89843994, 0.34941756, 0.10796044,
                          0.07166564, 0.67297414, 0.34139877, 0.56321003, 0.13224438,
                          0.58789568, 0.05265614, 0.93254668, 0.41326988, 0.67692951,
                          0.27922074, 0.38788297, 0.24478052, 0.29147   , 0.80741949,
                          0.67936156, 0.7442339 , 0.00343505, 0.97756934, 0.02268554,
                          0.56302353, 0.10718293, 0.11474464, 0.10465633, 0.04846449,
                          0.33695467, 0.43787266, 0.15092164, 0.80017919, 0.40017523,
                          0.40391072, 0.65025117, 0.3018835 , 0.15825793, 0.02963411,
                          0.85526189, 0.29678796, 0.23667277, 0.4013067 , 0.76988912,
                          0.06110263, 0.66297631, 0.79827956, 0.70776264, 0.07467447,
                          0.89814767, 0.00308201, 0.7823472 , 0.38646676, 0.75957091,
                          0.47684411, 0.7398732 , 0.09206989, 0.02529722, 0.1859329 ,
                          0.8380139 , 0.33920514, 0.0689636 , 0.07697459, 0.88068415,
                          0.1229827 , 0.89652486, 0.13968141, 0.38800301, 0.7728669 ,
                          0.16905682, 0.47354036, 0.90279152, 0.62008568, 0.82696116,
                          0.43869547, 0.94940345, 0.12938034, 0.20523408, 0.11727954,
                          0.54592836, 0.82115919, 0.96255349, 0.04999854, 0.15256932,
                          0.62537849, 0.15516518, 0.25683723, 0.85702076, 0.7925628 ,
                          0.46399241, 0.10106241, 0.73089281, 0.46200846, 0.24160109,
                          0.01349364, 0.94349643, 0.70886053, 0.06715038, 0.95042753,
                          0.38413263, 0.61285658, 0.97690412, 0.07900655, 0.21037925,
                          0.03351281, 0.36733596, 0.05601802, 0.50752553, 0.62088055,
                          0.94638543, 0.31649186, 0.19788369, 0.59813263, 0.31156879,
                          0.84129622, 0.18756002, 0.80252603, 0.44583102, 0.08424927,
                          0.8055779 , 0.89467745, 0.32244817, 0.5244238 , 0.38246742,
                          0.17552342, 0.09374914, 0.02755403, 0.86455687, 0.45570292,
                          0.58901182, 0.11888058, 0.65051228, 0.9634849 , 0.72370701,
                          0.6882061 , 0.30785926, 0.13060746, 0.29416438, 0.87322017,
                          0.26415365, 0.41275749, 0.44246432, 0.53266346, 0.0943344 ,
                          0.30480514, 0.37707017, 0.41691054, 0.94780656, 0.48190006,
                          0.55313378, 0.34750865, 0.4482111 , 0.62723585, 0.72810975,
                          0.05039657, 0.27579548, 0.03891394, 0.25236345, 0.53330415,
                          0.73508523, 0.07895291, 0.17533722, 0.35439847, 0.35161594,
                          0.56198773, 0.09715776, 0.65962064, 0.93017981, 0.69473252,
                          0.36962137, 0.01256946, 0.76358204, 0.71278475, 0.90981899,
                          0.12157694, 0.72489524, 0.35040793, 0.67236183, 0.70497179,
                          0.65668193, 0.42266533, 0.45514152, 0.13569985, 0.70177277,
                          0.47524764, 0.61467852, 0.7338517 , 0.54271988, 0.0514032 ,
                          0.75535566, 0.65658309, 0.01198156, 0.99577374, 0.22789359,
                          0.54174149, 0.25282717, 0.08002466, 0.26660684, 0.07484457,
                          0.87263561, 0.39011271, 0.93760461, 0.80665246, 0.38182704,
                          0.97697037, 0.89624951, 0.83816689, 0.39032672, 0.68852691,
                          0.85427299, 0.66773948, 0.9883756 , 0.06231242, 0.87223773,
                          0.12315628, 0.6709966 , 0.69840404, 0.19659599, 0.89613321,
                          0.79136648, 0.223493  , 0.8223016 , 0.81438973, 0.89321441,
                          0.82446078, 0.29317171, 0.23707863, 0.61798678, 0.85119219,
                          0.48374624, 0.18401164, 0.91021346, 0.18338856, 0.33581014,
                          0.51203974, 0.27373114, 0.26494583, 0.3909069 , 0.69500561,
                          0.44669268, 0.57792494, 0.92083106, 0.3760086 , 0.92086847,
                          0.98019746, 0.00930665, 0.91236066, 0.10589444, 0.98849831,
                          0.9937718 , 0.88560407, 0.20240646, 0.53980435, 0.21197806,
                          0.73511026, 0.91531094, 0.24419261, 0.79892127, 0.76623351,
                          0.32237578, 0.74371048, 0.89283081, 0.99471695, 0.59218713,
                          0.14229807, 0.26866107, 0.61418273, 0.53238885, 0.2847934 ,
                          0.33879263, 0.15419413, 0.81211755, 0.55982182, 0.33033445,
                          0.98925566, 0.21407401, 0.75933437, 0.50981508, 0.84659468,
                          0.27123332, 0.30602554, 0.78974943, 0.15961765, 0.75269879,
                          0.88404004, 0.25359787, 0.67575388, 0.10753205, 0.52492257,
                          0.2276367 , 0.57348205, 0.55631533, 0.48828726, 0.80950892,
                          0.68959411, 0.06038109, 0.3730253 , 0.44658293, 0.12323353,
                          0.90588169, 0.13484593, 0.58743073, 0.60592698, 0.67315081,
                          0.59887062, 0.3524358 , 0.47446065, 0.98078295, 0.31889862,
                          0.88225427, 0.81911728, 0.53942069, 0.6742203 , 0.73162166,
                          0.34597118, 0.70844054, 0.25029322, 0.29910746, 0.35906746,
                          0.53989701, 0.36776386, 0.04466711, 0.09399784, 0.53547235,
                          0.64757585, 0.03797524, 0.66378485, 0.34908186, 0.79601534,
                          0.10335962, 0.30468185, 0.57992791, 0.97139889, 0.40799129,
                          0.72985792, 0.65705408, 0.48913045, 0.46000752, 0.99260624,
                          0.52711571, 0.9383317 , 0.87126459, 0.16266698, 0.17428769,
                          0.11933665, 0.15703581, 0.17907467, 0.32411207, 0.56666047,
                          0.80868794, 0.49001672, 0.77590492, 0.63564239, 0.92169564,
                          0.5098178 , 0.40486284, 0.42978249, 0.4228141 , 0.33155423,
                          0.92695095, 0.40583767, 0.6635788 , 0.93854154, 0.48653097,
                          0.9305096 , 0.96009097, 0.03602708, 0.16874548, 0.4733966 ,
                          0.42363966, 0.18261131, 0.42653311, 0.48740795, 0.40008523,
                          0.35099519, 0.9641026 , 0.93447868, 0.16069199, 0.63925304,
                          0.17770585, 0.53600886, 0.72037108, 0.8454436 , 0.182311  ,
                          0.97860041, 0.41959913, 0.45109368, 0.24313804, 0.17884554,
                          0.04705525, 0.83247529, 0.89877392, 0.57362423, 0.27708354,
                          0.93649503, 0.43493419, 0.5422893 , 0.85565473, 0.86814896,
                          0.75788182, 0.02102082, 0.62643473, 0.24955471, 0.12775553,
                          0.96546452, 0.11001835, 0.82845919, 0.84811548, 0.21516077,
                          0.88871084, 0.55331041, 0.99744447, 0.70181741, 0.09492537,
                          0.18130881, 0.45487527, 0.82986703, 0.31207231, 0.08682494,
                          0.90971212, 0.40231716, 0.95428082, 0.10105085, 0.7243062 ,
                          0.87386255, 0.28549753, 0.90084605, 0.91034781, 0.44687279,
                          0.99318239, 0.58953929, 0.73074143, 0.05055378 };

    parameterVector D = { 0.146517  , 0.3707709 , 0.08328664, 0.49256865, 0.69405928,
                          0.61837293, 0.47376479, 0.11188706, 0.2228905 , 0.36340832,
                          0.0693854 , 0.10615699, 0.88785036, 0.6677967 , 0.63177135,
                          0.57125641, 0.80023305, 0.08298528, 0.91838322, 0.18315431,
                          0.89992512, 0.53899603, 0.41261589, 0.31495081, 0.83742576,
                          0.09300794, 0.82360698, 0.21493177, 0.1629844 , 0.21152065,
                          0.16961513, 0.4914438 , 0.50520605, 0.14553687, 0.26358359,
                          0.75964966, 0.65752746, 0.71537866, 0.0052241 , 0.96884752,
                          0.39184445, 0.9992278 , 0.82985355, 0.77985287, 0.82259158,
                          0.40209148, 0.65923576, 0.86734155, 0.82929213, 0.45634802,
                          0.27689889, 0.96708886, 0.1655354 , 0.89114231, 0.19536992,
                          0.33743959, 0.73300973, 0.363747  , 0.26554793, 0.61940794,
                          0.68798717, 0.31331865, 0.29169741, 0.94606427, 0.57961661,
                          0.9410199 , 0.09121453, 0.42521405, 0.67622819, 0.69165347,
                          0.84437122, 0.21535456, 0.15999728, 0.34674508, 0.86254606,
                          0.04035766, 0.19320383, 0.05731681, 0.95023339, 0.38534862,
                          0.01313969 };

    variableVector answerPK2Stress = { 314.71116295,   30.09663087,  -97.50775502,
                                       275.16187648,   28.28022151,  -85.63390628,
                                       345.9794607 ,   29.69720043, -103.26896348 };

    variableVector answerMicroStress = { 630.03379183,  306.44161512,  248.73797179,
                                         306.21515887,   59.80476285,  -54.44381993,
                                         251.26329613,  -53.54715409, -206.44832422 };

    variableVector answerHigherOrderStress = { -6.34180841, -8.44235442, -7.66602685, -6.62791667, -6.30156652,
                                               -7.81093903, -9.08319118, -7.62283755, -8.7120047 , -7.96533995,
                                               -7.29110914, -7.63480242, -6.0360827 , -6.66816385, -6.38308499,
                                               -8.06776472, -7.29777722, -7.77952498, -7.6470537 , -9.94159411,
                                               -7.65257834, -5.90193479, -6.5591572 , -8.12839975, -8.56024681,
                                               -7.40823637, -8.875604 };

    variableVector resultPK2Stress, resultMicroStress, resultHigherOrderStress;

    errorOut error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient, microDeformation,
                                                                              gradientMicroDeformation,
                                                                              A, B, C, D,
                                                                              resultPK2Stress, resultMicroStress,
                                                                              resultHigherOrderStress );

    if ( error ){
        error->print();
        results << "test_linearElasticityReference & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultPK2Stress, answerPK2Stress ) ){
        results << "test_linearElasticityReference (test 1) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultMicroStress, answerMicroStress ) ){
        results << "test_linearElasticityReference (test 2) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultHigherOrderStress, answerHigherOrderStress ) ){
        results << "test_linearElasticityReference (test 3) & False\n";
        return 1;
    }

    //Test the Jacobians

    variableVector resultJPK2Stress, resultJMicroStress, resultJHigherOrderStress;
    variableMatrix dPK2dF, dPK2dXi, dPK2dGradXi, dSigmadF, dSigmadXi, dSigmadGradXi, dMdF, dMdGradXi;

    error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient, microDeformation,
                                                                     gradientMicroDeformation,
                                                                     A, B, C, D,
                                                                     resultJPK2Stress, resultJMicroStress, resultJHigherOrderStress,
                                                                     dPK2dF, dPK2dXi, dPK2dGradXi, dSigmadF, dSigmadXi, dSigmadGradXi,
                                                                     dMdF, dMdGradXi );

    if ( error ){
        error->print();
        results << "test_linearElasticityReference & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJPK2Stress, answerPK2Stress ) ){
        results << "test_linearElasticityReference (test 4) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJMicroStress, answerMicroStress ) ){
        results << "test_linearElasticityReference (test 5) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJHigherOrderStress, answerHigherOrderStress ) ){
        results << "test_linearElasticityReference (test 6) & False\n";
        return 1;
    }

    //Test Jacobians w.r.t. the deformation gradient
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < deformationGradient.size(); i++ ){
        constantVector delta( deformationGradient.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector PK2_P, PK2_M;
        variableVector Sigma_P, Sigma_M;
        variableVector M_P, M_M;

        error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient + delta, microDeformation,
                                                                         gradientMicroDeformation,
                                                                         A, B, C, D,
                                                                         PK2_P, Sigma_P, M_P );

        if ( error ){
            error->print();
            results << "test_linearElasticityReference & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient - delta, microDeformation,
                                                                         gradientMicroDeformation,
                                                                         A, B, C, D,
                                                                         PK2_M, Sigma_M, M_M );

        if ( error ){
            error->print();
            results << "test_linearElasticityReference & False\n";
            return 1;
        }

        constantVector gradCol = ( PK2_P - PK2_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dPK2dF[j][i] ) ){
                results << "test_linearElasticityReference (test 7) & False\n";
                return 1;
            }
        }

        gradCol = ( Sigma_P - Sigma_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dSigmadF[j][i] ) ){
                results << "test_linearElasticityReference (test 8) & False\n";
                return 1;
            }
        }

        gradCol = ( M_P - M_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMdF[j][i] ) ){
                results << "test_linearElasticityReference (test 9) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the micro deformation
    for ( unsigned int i = 0; i < microDeformation.size(); i++ ){
        constantVector delta( microDeformation.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector PK2_P, PK2_M;
        variableVector Sigma_P, Sigma_M;
        variableVector M_P, M_M;

        error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient, microDeformation + delta,
                                                                         gradientMicroDeformation,
                                                                         A, B, C, D,
                                                                         PK2_P, Sigma_P, M_P );

        if ( error ){
            error->print();
            results << "test_linearElasticityReference & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient, microDeformation - delta,
                                                                         gradientMicroDeformation,
                                                                         A, B, C, D,
                                                                         PK2_M, Sigma_M, M_M );

        if ( error ){
            error->print();
            results << "test_linearElasticityReference & False\n";
            return 1;
        }

        constantVector gradCol = ( PK2_P - PK2_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dPK2dXi[j][i] ) ){
                results << "test_linearElasticityReference (test 10) & False\n";
                return 1;
            }
        }

        gradCol = ( Sigma_P - Sigma_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dSigmadXi[j][i] ) ){
                results << "test_linearElasticityReference (test 11) & False\n";
                return 1;
            }
        }

        gradCol = ( M_P - M_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[ j ], 0. ) ){
                results << "test_linearElasticityReference (test 12) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the gradient of the micro deformation
    for ( unsigned int i = 0; i < gradientMicroDeformation.size(); i++ ){
        constantVector delta( gradientMicroDeformation.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector PK2_P, PK2_M;
        variableVector Sigma_P, Sigma_M;
        variableVector M_P, M_M;

        error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient, microDeformation,
                                                                         gradientMicroDeformation + delta,
                                                                         A, B, C, D,
                                                                         PK2_P, Sigma_P, M_P );

        if ( error ){
            error->print();
            results << "test_linearElasticityReference & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::linearElasticityReference( deformationGradient, microDeformation,
                                                                         gradientMicroDeformation - delta,
                                                                         A, B, C, D,
                                                                         PK2_M, Sigma_M, M_M );

        if ( error ){
            error->print();
            results << "test_linearElasticityReference & False\n";
            return 1;
        }

        constantVector gradCol = ( PK2_P - PK2_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dPK2dGradXi[j][i] ) ){
                results << "test_linearElasticityReference (test 13) & False\n";
                return 1;
            }
        }

        gradCol = ( Sigma_P - Sigma_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dSigmadGradXi[j][i], 1e-4 ) ){
                results << "test_linearElasticityReference (test 14) & False\n";
                return 1;
            }
        }

        gradCol = ( M_P - M_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMdGradXi[j][i], 1e-4 ) ){
                results << "test_linearElasticityReference (test 15) & False\n";
                return 1;
            }
        }
    }

    results << "test_linearElasticityReference & True\n";
    return 0;
}

int test_computeInvRCGPsi( std::ofstream &results ){
    /*!
     * Test the computation of the invRCG Psi product
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector RCG = { 0.34852835, 0.47540122, 1.11252634,
                           0.47540122, 1.49184663, 1.57435946,
                           1.11252634, 1.57435946, 3.68235756 };

    variableVector Psi = { -0.02303102, -0.41101265, -0.36040573,
                           -0.14715403, -0.18045474, -0.8814645 ,
                           -0.36637526, -1.08887072, -1.44707636 };

    variableVector invRCG = vectorTools::inverse( RCG, 3, 3 );

    variableVector answer = { 7.06496448, -6.60478112,  6.18226067,
                              0.01374041,  0.34618158, -0.31907041,
                             -2.23986034,  1.55175262, -2.12436531 };

    variableVector result;
    errorOut error = micromorphicLinearElasticity::computeInvRCGPsi( invRCG, Psi, result );

    if ( error ){
        results << "test_computeInvRCGPsi & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( result, answer ) ){
        results << "test_computeInvRCGPsi (test 1) & False\n";
        return 1;
    }

    //Test Jacobians

    variableVector resultJ;
    variableMatrix dInvRCGPsidRCG, dInvRCGPsidPsi;

    error = micromorphicLinearElasticity::computeInvRCGPsi( invRCG, Psi, resultJ, dInvRCGPsidRCG, dInvRCGPsidPsi );

    if ( error ){
        error->print();
        results << "test_computeInvRCGPsi & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJ, answer ) ){
        results << "test_computeInvRCGPsi (test 2) & False\n";
        return 1;
    }

    // Test dInvRCGPsidRCG
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < RCG.size(); i++ ){
        constantVector delta( RCG.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector result_P, result_M;

        variableVector invRCG_P = vectorTools::inverse( RCG + delta, 3, 3 );

        error = micromorphicLinearElasticity::computeInvRCGPsi( invRCG_P, Psi, result_P );

        if ( error ){
            error->print();
            results << "test_computeInvRCGPsi & False\n";
            return 1;
        }

        variableVector invRCG_M = vectorTools::inverse( RCG - delta, 3, 3 );

        error = micromorphicLinearElasticity::computeInvRCGPsi( invRCG_M, Psi, result_M );

        if ( error ){
            error->print();
            results << "test_computeInvRCGPsi & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dInvRCGPsidRCG[j][i] ) ){
                results << "test_computeInvRCGPsi (test 3) & False\n";
                return 1;
            }
        }
    }

    // Test dInvRCGPsidPsi
    for ( unsigned int i = 0; i < Psi.size(); i++ ){
        constantVector delta( Psi.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector result_P, result_M;

        error = micromorphicLinearElasticity::computeInvRCGPsi( invRCG, Psi + delta, result_P );

        if ( error ){
            error->print();
            results << "test_computeInvRCGPsi & False\n";
            return 1;
        }

        error = micromorphicLinearElasticity::computeInvRCGPsi( invRCG, Psi - delta, result_M );

        if ( error ){
            error->print();
            results << "test_computeInvRCGPsi & False\n";
            return 1;
        }

        constantVector gradCol = ( result_P - result_M ) / ( 2 * delta[ i ] );

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dInvRCGPsidPsi[j][i] ) ){
                results << "test_computeInvRCGPsi (test 4) & False\n";
                return 1;
            }
        }
    }

    results << "test_computeInvRCGPsi & True\n";
    return 0;
}

int test_computeInvRCGGamma( std::ofstream &results ){
    /*!
     * Test the computation of the invRCG Gamma product
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector RCG = { 0.34852835, 0.47540122, 1.11252634,
                           0.47540122, 1.49184663, 1.57435946,
                           1.11252634, 1.57435946, 3.68235756 };

    variableVector Gamma = { -0.31120922, -0.3563267 , -0.36573233, -0.0771914 , -0.24252804,
                             -0.4738459 , -0.35937075, -0.01781817, -0.17465609, -0.82225557,
                             -0.36719542, -0.86494826, -0.92750732, -1.18214541, -1.00423785,
                             -0.43125133, -0.19543115, -0.49736256, -0.67098335, -0.98433811,
                             -1.0183107 , -0.12645195, -0.79818076, -1.23318541, -0.95577138,
                              0.1274431 , -0.47648617 };

    variableVector invRCG = vectorTools::inverse( RCG, 3, 3 );


    variableVector answer = { -8.75662978, -4.74843358, -4.6911341 , -3.16355851, -0.13179485,
                              -8.17350013, -5.69120721, -4.54526807, -2.48129262, -0.65657733,
                               0.06407926, -0.52611055, -1.06777508, -1.02709034, -0.58509241,
                              -0.02936234, -0.30663195, -0.35940994,  2.74408079,  1.13990438,
                               1.36569754,  1.37796288,  0.26218363,  2.38466645,  1.47244621,
                               1.53893867,  0.77392204 };

    variableVector result;
    errorOut error = micromorphicLinearElasticity::computeInvRCGGamma( invRCG, Gamma, result );

    if ( error ){
        results << "test_computeInvRCGGamma & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( result, answer ) ){
        results << "test_computeInvRCGGamma (test 1) & False\n";
        return 1;
    }

    //Test Jacobians

    variableVector resultJ;
    variableMatrix dInvRCGGammadRCG, dInvRCGGammadGamma;

    error = micromorphicLinearElasticity::computeInvRCGGamma( invRCG, Gamma, resultJ, dInvRCGGammadRCG, dInvRCGGammadGamma );

    if ( error ){
        error->print();
        results << "test_computeInvRCGGamma & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJ, answer ) ){
        results << "test_computeInvRCGGamma (test 2) & False\n";
        return 1;
    }

    // Test dInvRCGGammadRCG
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < RCG.size(); i++ ){
        constantVector delta( RCG.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        variableVector invRCGJ = vectorTools::inverse( RCG + delta, 3, 3 );

        error = micromorphicLinearElasticity::computeInvRCGGamma( invRCGJ, Gamma, resultJ );

        if ( error ){
            error->print();
            results << "test_computeInvRCGGamma & False\n";
            return 1;
        }

        constantVector gradCol = ( resultJ - result ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dInvRCGGammadRCG[j][i], 1e-4 ) ){
                results << "test_computeInvRCGGamma (test 3) & False\n";
                return 1;
            }
        }
    }

    // Test dInvRCGGammadPsi
    for ( unsigned int i = 0; i < Gamma.size(); i++ ){
        constantVector delta( Gamma.size(), 0 );
        delta[i] = eps * fabs( delta[i] ) + eps;

        error = micromorphicLinearElasticity::computeInvRCGGamma( invRCG, Gamma + delta, resultJ );

        if ( error ){
            error->print();
            results << "test_computeInvRCGGamma & False\n";
            return 1;
        }

        constantVector gradCol = ( resultJ - result ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dInvRCGGammadGamma[j][i] ) ){
                results << "test_computeInvRCGGamma (test 4) & False\n";
                return 1;
            }
        }
    }

    results << "test_computeInvRCGGamma & True\n";
    return 0;
}

int test_mapStressesToCurrent( std::ofstream &results ){
    /*!
     * Test mapping the stresses from the reference configuration 
     * to the current configuration.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector PK2Stress = { 314.71116295,   30.09663087,  -97.50775502,
                                 275.16187648,   28.28022151,  -85.63390628,
                                 345.9794607 ,   29.69720043, -103.26896348 };

    variableVector referenceMicroStress = { 630.03379183,  306.44161512,  248.73797179,
                                            306.21515887,   59.80476285,  -54.44381993,
                                            251.26329613,  -53.54715409, -206.44832422 };

    variableVector referenceHigherOrderStress = { -6.34180841, -8.44235442, -7.66602685, -6.62791667, -6.30156652,
                                                  -7.81093903, -9.08319118, -7.62283755, -8.7120047 , -7.96533995,
                                                  -7.29110914, -7.63480242, -6.0360827 , -6.66816385, -6.38308499,
                                                  -8.06776472, -7.29777722, -7.77952498, -7.6470537 , -9.94159411,
                                                  -7.65257834, -5.90193479, -6.5591572 , -8.12839975, -8.56024681,
                                                  -7.40823637, -8.875604 };

    variableVector deformationGradient = { -0.50668478, -0.48303615, -1.43907185,
                                            0.24106815,  0.10656166,  1.06851718,
                                           -0.18353482, -1.11676646, -0.68534721 };

    variableVector microDeformation = { -0.36302711,  0.94624033,  0.07877948,
                                        -0.68872716,  0.10276167, -0.81357538,
                                         0.22307023, -0.23788599,  0.67759487 };

    variableVector answerCauchyStress = { -468.08486373, -298.02079112, -315.35069455,
                                           285.12093132,  174.8563172 ,  186.50558111,
                                          -349.70100327, -236.00001236, -250.10967988 };

    variableVector answerMicroStress = { -970.04337968,    1.59908878, -705.45138092,
                                            5.89339453,  342.16732824,  -26.91143239,
                                         -700.06232934,  -32.80059522, -541.08302059 };

    variableVector answerHigherOrderStress = { 152.3275986 , -345.32313068,  168.48941995,  -87.55872352,
                                               207.72901358, -101.23620149,  118.78091452, -265.95494635,
                                               132.87071502,  -90.27961059,  203.28666944,  -99.52254888,
                                                51.72482788, -122.30453021,   59.79814044,  -70.00039233,
                                               157.01422352,  -79.06428707,  120.23269317, -274.19301168,
                                               132.51680884,  -69.47891023,  164.77681737,  -79.65682209,
                                                95.84907668, -208.69345804,  102.08397015 };

    variableVector resultCauchyStress, resultMicroStress, resultHigherOrderStress;

    errorOut error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient, microDeformation,
                                                                               PK2Stress, referenceMicroStress,
                                                                               referenceHigherOrderStress, resultCauchyStress,
                                                                               resultMicroStress, resultHigherOrderStress );

    if ( error ){
        error->print();
        results << "test_mapStressesToCurrent & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultCauchyStress, answerCauchyStress, 1e-5 ) ){
        results << "test_mapStressesToCurrent (test 1) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultMicroStress, answerMicroStress, 1e-5 ) ){
        results << "test_mapStressesToCurrent (test 2) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultHigherOrderStress, answerHigherOrderStress, 1e-5 ) ){
        results << "test_mapStressesToCurrent (test 3) & False\n";
        return 1;
    }

    //Test the Jacobians

    variableVector resultJCauchyStress, resultJMicroStress, resultJHigherOrderStress;

    variableMatrix dCauchyStressdF, dCauchyStressdPK2Stress;
    variableMatrix dMicroStressdF, dMicroStressdReferenceMicroStress;
    variableMatrix dHigherOrderStressdF, dHigherOrderStressdXi, dHigherOrderStressdReferenceHigherOrderStress;

    error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient, microDeformation,
                                                                      PK2Stress, referenceMicroStress,
                                                                      referenceHigherOrderStress, resultJCauchyStress,
                                                                      resultJMicroStress, resultJHigherOrderStress,
                                                                      dCauchyStressdF, dCauchyStressdPK2Stress,
                                                                      dMicroStressdF, dMicroStressdReferenceMicroStress,
                                                                      dHigherOrderStressdF, dHigherOrderStressdXi,
                                                                      dHigherOrderStressdReferenceHigherOrderStress );

    if ( error ){
        error->print();
        results << "test_mapStressesToCurrent & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJCauchyStress, answerCauchyStress, 1e-5 ) ){
        results << "test_mapStressesToCurrent (test 4) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJMicroStress, answerMicroStress, 1e-5 ) ){
        results << "test_mapStressesToCurrent (test 5) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJHigherOrderStress, answerHigherOrderStress, 1e-5 ) ){
        results << "test_mapStressesToCurrent (test 6) & False\n";
        return 1;
    }

    //Test Jacobians w.r.t. the deformation gradient
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < deformationGradient.size(); i++ ){
        constantVector delta( deformationGradient.size(), 0 );
        delta[i] = eps * fabs( deformationGradient[i] ) + eps;

        error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient + delta, microDeformation,
                                                                          PK2Stress, referenceMicroStress,
                                                                          referenceHigherOrderStress, resultJCauchyStress,
                                                                          resultJMicroStress, resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_mapStressesToCurrent & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dCauchyStressdF[j][i], 1e-5 ) ){
                results << "test_mapStressesToCurrent (test 7) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMicroStressdF[j][i], 1e-5 ) ){
                results << "test_mapStressesToCurrent (test 8) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dHigherOrderStressdF[j][i], 1e-4 ) ){
                results << "test_mapStressesToCurrent (test 9) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the micro-deformation
    for ( unsigned int i = 0; i < microDeformation.size(); i++ ){
        constantVector delta( microDeformation.size(), 0 );
        delta[i] = eps * fabs( microDeformation[i] ) + eps;

        error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient, microDeformation + delta,
                                                                          PK2Stress, referenceMicroStress,
                                                                          referenceHigherOrderStress, resultJCauchyStress,
                                                                          resultJMicroStress, resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_mapStressesToCurrent & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0., 1e-5 ) ){
                results << "test_mapStressesToCurrent (test 10) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0., 1e-5 ) ){
                results << "test_mapStressesToCurrent (test 11) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dHigherOrderStressdXi[j][i], 1e-5 ) ){
                results << "test_mapStressesToCurrent (test 12) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the PK2 Stress
    for ( unsigned int i = 0; i < PK2Stress.size(); i++ ){
        constantVector delta( PK2Stress.size(), 0 );
        delta[i] = eps * fabs( PK2Stress[i] ) + eps;

        error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient, microDeformation,
                                                                          PK2Stress + delta, referenceMicroStress,
                                                                          referenceHigherOrderStress, resultJCauchyStress,
                                                                          resultJMicroStress, resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_mapStressesToCurrent & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dCauchyStressdPK2Stress[j][i] ) ){
                results << "test_mapStressesToCurrent (test 13) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_mapStressesToCurrent (test 14) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_mapStressesToCurrent (test 15) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the reference micro-stress
    for ( unsigned int i = 0; i < referenceMicroStress.size(); i++ ){
        constantVector delta( referenceMicroStress.size(), 0 );
        delta[i] = eps * fabs( referenceMicroStress[i] ) + eps;

        error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient, microDeformation,
                                                                          PK2Stress, referenceMicroStress + delta,
                                                                          referenceHigherOrderStress, resultJCauchyStress,
                                                                          resultJMicroStress, resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_mapStressesToCurrent & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_mapStressesToCurrent (test 16) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMicroStressdReferenceMicroStress[j][i] ) ){
                results << "test_mapStressesToCurrent (test 17) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_mapStressesToCurrent (test 18) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the reference higher-order stress
    for ( unsigned int i = 0; i < referenceHigherOrderStress.size(); i++ ){
        constantVector delta( referenceHigherOrderStress.size(), 0 );
        delta[i] = eps * fabs( referenceHigherOrderStress[i] ) + eps;

        error = micromorphicLinearElasticity::mapStressMeasuresToCurrent( deformationGradient, microDeformation,
                                                                          PK2Stress, referenceMicroStress,
                                                                          referenceHigherOrderStress + delta, resultJCauchyStress,
                                                                          resultJMicroStress, resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_mapStressesToCurrent & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_mapStressesToCurrent (test 19) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], 0. ) ){
                results << "test_mapStressesToCurrent (test 20) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dHigherOrderStressdReferenceHigherOrderStress[j][i] ) ){
                results << "test_mapStressesToCurrent (test 21) & False\n";
                return 1;
            }
        }
    }

    results << "test_mapStressesToCurrent & True\n";
    return 0;
}

int test_linearElasticity( std::ofstream &results ){
    /*!
     * Test the micro-morphic linear elasticity model where the 
     * stresses are pushed to the current configuration.
     *
     * :param std::ofstream &results: The output file.
     */

    variableVector deformationGradient = { -0.50668478, -0.48303615, -1.43907185,
                                            0.24106815,  0.10656166,  1.06851718,
                                           -0.18353482, -1.11676646, -0.68534721 };

    variableVector microDeformation = { -0.36302711,  0.94624033,  0.07877948,
                                        -0.68872716,  0.10276167, -0.81357538,
                                         0.22307023, -0.23788599,  0.67759487 };

    variableVector gradientMicroDeformation = { 0.80507219, 0.73460211, 0.66571977, 0.13571332, 0.18559912,
                                                0.99230253, 0.77887526, 0.23648914, 0.31711178, 0.75118698,
                                                0.08013972, 0.27232507, 0.59595994, 0.13892773, 0.51533812,
                                                0.19823639, 0.51598785, 0.19048906, 0.45974189, 0.01871104,
                                                0.51255207, 0.82869552, 0.99152216, 0.51920895, 0.06818867,
                                                0.12194391, 0.32637525 };

    parameterVector A = { 0.91738548, 0.5223949 , 0.04303308, 0.42138619, 0.71420722,
                          0.15443589, 0.05041408, 0.69624665, 0.8766614 , 0.17261697,
                          0.02350474, 0.59406857, 0.79573586, 0.21213138, 0.47821637,
                          0.35462425, 0.6291708 , 0.48565385, 0.67132896, 0.27926608,
                          0.04579313, 0.88556864, 0.08992741, 0.75316186, 0.76279627,
                          0.5635193 , 0.18529158, 0.05722408, 0.65275234, 0.97189144,
                          0.68435   , 0.96624106, 0.84374092, 0.21040392, 0.13887068,
                          0.34423717, 0.50801461, 0.28726825, 0.52590869, 0.36090934,
                          0.97602275, 0.95087184, 0.32716562, 0.50074403, 0.3955299 ,
                          0.48018626, 0.71150853, 0.61899609, 0.57103915, 0.5154469 ,
                          0.4456661 , 0.41192121, 0.12649935, 0.69069678, 0.17931527,
                          0.98427378, 0.82378172, 0.44572395, 0.90859147, 0.33180413,
                          0.30215348, 0.42929583, 0.61595281, 0.66534843, 0.31552903,
                          0.99326382, 0.87708958, 0.27827411, 0.30275486, 0.3209769 ,
                          0.81059907, 0.83577572, 0.54758756, 0.30482114, 0.00502004,
                          0.09242907, 0.24196602, 0.00779042, 0.04284832, 0.56224798,
                          0.86563423 };

    parameterVector B = { 0.99402085, 0.66339725, 0.73962847, 0.75991152, 0.91213988,
                          0.94984965, 0.6011931 , 0.32834193, 0.21231827, 0.65420996,
                          0.66533091, 0.83293238, 0.2537865 , 0.57946922, 0.79358565,
                          0.92885037, 0.0923514 , 0.12441041, 0.87678012, 0.87730359,
                          0.59116472, 0.21901437, 0.45976152, 0.86524067, 0.06668473,
                          0.83812813, 0.06951684, 0.91636315, 0.07878975, 0.3887551 ,
                          0.86632579, 0.84909984, 0.72558761, 0.12280263, 0.92078995,
                          0.48305302, 0.19393044, 0.82984994, 0.27981095, 0.60669024,
                          0.25483571, 0.2663953 , 0.3504269 , 0.50945399, 0.15647713,
                          0.46803744, 0.44503108, 0.86396469, 0.44057713, 0.97430525,
                          0.12593544, 0.48379355, 0.82188636, 0.31297267, 0.21986847,
                          0.8032502 , 0.07287772, 0.6974731 , 0.97608146, 0.49354539,
                          0.97886936, 0.16204678, 0.79205693, 0.81275365, 0.44670719,
                          0.22577849, 0.14381145, 0.92846116, 0.13905519, 0.36151037,
                          0.71576657, 0.5462745 , 0.41204395, 0.79415741, 0.73346637,
                          0.51829857, 0.31806782, 0.73860407, 0.21137896, 0.06281619,
                          0.08517056 };

    parameterVector C = { 0.73168423, 0.20787282, 0.44597068, 0.13971472, 0.67623962,
                          0.01024113, 0.26235898, 0.52638878, 0.55822266, 0.79169357,
                          0.79556295, 0.26274848, 0.22158735, 0.77447856, 0.09905053,
                          0.24537506, 0.82422833, 0.13912553, 0.13704714, 0.60418098,
                          0.97916951, 0.96975567, 0.15156735, 0.33820056, 0.40984014,
                          0.03316931, 0.07805217, 0.09086063, 0.0587449 , 0.93973718,
                          0.2088402 , 0.27030923, 0.90893679, 0.45495913, 0.38558114,
                          0.89599555, 0.99117823, 0.68663521, 0.47807759, 0.47462775,
                          0.63545614, 0.76359775, 0.58543208, 0.67889697, 0.23400169,
                          0.90355814, 0.81277492, 0.27582141, 0.34052255, 0.09211961,
                          0.38139711, 0.40588835, 0.72562255, 0.46843548, 0.53552493,
                          0.10983976, 0.70222301, 0.33678326, 0.5755877 , 0.25931049,
                          0.4508404 , 0.61309158, 0.93828205, 0.54132972, 0.74656135,
                          0.66254458, 0.79255496, 0.15105507, 0.73739719, 0.8699253 ,
                          0.30037259, 0.18970023, 0.7351555 , 0.19264707, 0.66209567,
                          0.96806941, 0.64975045, 0.54480589, 0.11455497, 0.48495469,
                          0.76967885, 0.23472007, 0.43837476, 0.95659971, 0.66906068,
                          0.06233808, 0.89998454, 0.61301419, 0.83415149, 0.08989232,
                          0.68242323, 0.5311455 , 0.04401578, 0.64268437, 0.17462098,
                          0.91385774, 0.3254543 , 0.05379172, 0.77842952, 0.53732987,
                          0.83781747, 0.7615337 , 0.62005118, 0.73293008, 0.82346972,
                          0.2179367 , 0.36317936, 0.55619014, 0.09312705, 0.4694652 ,
                          0.69716073, 0.02440034, 0.53107043, 0.97073677, 0.87477045,
                          0.68304994, 0.78714746, 0.23263201, 0.42970537, 0.72939955,
                          0.12454156, 0.63073136, 0.31116734, 0.39634253, 0.1281059 ,
                          0.23014438, 0.13811186, 0.23720232, 0.16020445, 0.10732273,
                          0.90941219, 0.89549355, 0.71971555, 0.01863396, 0.63512323,
                          0.57834974, 0.74377666, 0.67809585, 0.69763216, 0.75809662,
                          0.34523363, 0.42721884, 0.06343445, 0.4707797 , 0.91267953,
                          0.9522137 , 0.429135  , 0.94506974, 0.42002389, 0.48045774,
                          0.1877623 , 0.93541748, 0.10358528, 0.90585229, 0.94482345,
                          0.85486823, 0.71201071, 0.82729667, 0.67685002, 0.89074951,
                          0.13603059, 0.29549921, 0.5829021 , 0.85710379, 0.33597495,
                          0.2635317 , 0.54822056, 0.13518258, 0.07510343, 0.57277576,
                          0.66026008, 0.10590873, 0.40988651, 0.73181046, 0.21849923,
                          0.68193615, 0.4861005 , 0.90062638, 0.49503759, 0.53109181,
                          0.31197913, 0.8260051 , 0.56845431, 0.20510746, 0.48927707,
                          0.84796951, 0.57021869, 0.32869802, 0.00649644, 0.89085066,
                          0.58793337, 0.13725509, 0.49166181, 0.79467837, 0.55550476,
                          0.86168924, 0.26284446, 0.34931772, 0.69039842, 0.04226658,
                          0.91252659, 0.8532767 , 0.15745086, 0.11244899, 0.35188228,
                          0.66119509, 0.88971845, 0.90199259, 0.53564388, 0.08103036,
                          0.89537074, 0.43988547, 0.39234971, 0.90744335, 0.87819375,
                          0.25940274, 0.48165619, 0.08404158, 0.16900508, 0.20502448,
                          0.00336955, 0.94376888, 0.89722214, 0.06817336, 0.35272289,
                          0.34452052, 0.23363246, 0.79650105, 0.8107239 , 0.94490429,
                          0.26741852, 0.87105166, 0.25525768, 0.26586211, 0.6449152 ,
                          0.10839033, 0.6871309 , 0.59008043, 0.07558712, 0.99527881,
                          0.13052048, 0.81075174, 0.38967993, 0.25408067, 0.78035165,
                          0.48123955, 0.97775619, 0.50408867, 0.51411035, 0.17947261,
                          0.99740746, 0.84538866, 0.62373254, 0.38782162, 0.55585207,
                          0.24743969, 0.25980163, 0.50272755, 0.76170535, 0.338618  ,
                          0.33580793, 0.58798537, 0.13328799, 0.01026525, 0.13839967,
                          0.31984267, 0.72693472, 0.84737434, 0.97859975, 0.61637914,
                          0.23018791, 0.89805651, 0.69772024, 0.7491404 , 0.3818782 ,
                          0.50000777, 0.71398283, 0.29910862, 0.36270529, 0.93178041,
                          0.03156497, 0.57674924, 0.13573152, 0.59758916, 0.47467419,
                          0.21707829, 0.36305461, 0.58480959, 0.18659161, 0.20999611,
                          0.23732489, 0.11099326, 0.49055309, 0.52547794, 0.01722654,
                          0.19637688, 0.03560497, 0.89843994, 0.34941756, 0.10796044,
                          0.07166564, 0.67297414, 0.34139877, 0.56321003, 0.13224438,
                          0.58789568, 0.05265614, 0.93254668, 0.41326988, 0.67692951,
                          0.27922074, 0.38788297, 0.24478052, 0.29147   , 0.80741949,
                          0.67936156, 0.7442339 , 0.00343505, 0.97756934, 0.02268554,
                          0.56302353, 0.10718293, 0.11474464, 0.10465633, 0.04846449,
                          0.33695467, 0.43787266, 0.15092164, 0.80017919, 0.40017523,
                          0.40391072, 0.65025117, 0.3018835 , 0.15825793, 0.02963411,
                          0.85526189, 0.29678796, 0.23667277, 0.4013067 , 0.76988912,
                          0.06110263, 0.66297631, 0.79827956, 0.70776264, 0.07467447,
                          0.89814767, 0.00308201, 0.7823472 , 0.38646676, 0.75957091,
                          0.47684411, 0.7398732 , 0.09206989, 0.02529722, 0.1859329 ,
                          0.8380139 , 0.33920514, 0.0689636 , 0.07697459, 0.88068415,
                          0.1229827 , 0.89652486, 0.13968141, 0.38800301, 0.7728669 ,
                          0.16905682, 0.47354036, 0.90279152, 0.62008568, 0.82696116,
                          0.43869547, 0.94940345, 0.12938034, 0.20523408, 0.11727954,
                          0.54592836, 0.82115919, 0.96255349, 0.04999854, 0.15256932,
                          0.62537849, 0.15516518, 0.25683723, 0.85702076, 0.7925628 ,
                          0.46399241, 0.10106241, 0.73089281, 0.46200846, 0.24160109,
                          0.01349364, 0.94349643, 0.70886053, 0.06715038, 0.95042753,
                          0.38413263, 0.61285658, 0.97690412, 0.07900655, 0.21037925,
                          0.03351281, 0.36733596, 0.05601802, 0.50752553, 0.62088055,
                          0.94638543, 0.31649186, 0.19788369, 0.59813263, 0.31156879,
                          0.84129622, 0.18756002, 0.80252603, 0.44583102, 0.08424927,
                          0.8055779 , 0.89467745, 0.32244817, 0.5244238 , 0.38246742,
                          0.17552342, 0.09374914, 0.02755403, 0.86455687, 0.45570292,
                          0.58901182, 0.11888058, 0.65051228, 0.9634849 , 0.72370701,
                          0.6882061 , 0.30785926, 0.13060746, 0.29416438, 0.87322017,
                          0.26415365, 0.41275749, 0.44246432, 0.53266346, 0.0943344 ,
                          0.30480514, 0.37707017, 0.41691054, 0.94780656, 0.48190006,
                          0.55313378, 0.34750865, 0.4482111 , 0.62723585, 0.72810975,
                          0.05039657, 0.27579548, 0.03891394, 0.25236345, 0.53330415,
                          0.73508523, 0.07895291, 0.17533722, 0.35439847, 0.35161594,
                          0.56198773, 0.09715776, 0.65962064, 0.93017981, 0.69473252,
                          0.36962137, 0.01256946, 0.76358204, 0.71278475, 0.90981899,
                          0.12157694, 0.72489524, 0.35040793, 0.67236183, 0.70497179,
                          0.65668193, 0.42266533, 0.45514152, 0.13569985, 0.70177277,
                          0.47524764, 0.61467852, 0.7338517 , 0.54271988, 0.0514032 ,
                          0.75535566, 0.65658309, 0.01198156, 0.99577374, 0.22789359,
                          0.54174149, 0.25282717, 0.08002466, 0.26660684, 0.07484457,
                          0.87263561, 0.39011271, 0.93760461, 0.80665246, 0.38182704,
                          0.97697037, 0.89624951, 0.83816689, 0.39032672, 0.68852691,
                          0.85427299, 0.66773948, 0.9883756 , 0.06231242, 0.87223773,
                          0.12315628, 0.6709966 , 0.69840404, 0.19659599, 0.89613321,
                          0.79136648, 0.223493  , 0.8223016 , 0.81438973, 0.89321441,
                          0.82446078, 0.29317171, 0.23707863, 0.61798678, 0.85119219,
                          0.48374624, 0.18401164, 0.91021346, 0.18338856, 0.33581014,
                          0.51203974, 0.27373114, 0.26494583, 0.3909069 , 0.69500561,
                          0.44669268, 0.57792494, 0.92083106, 0.3760086 , 0.92086847,
                          0.98019746, 0.00930665, 0.91236066, 0.10589444, 0.98849831,
                          0.9937718 , 0.88560407, 0.20240646, 0.53980435, 0.21197806,
                          0.73511026, 0.91531094, 0.24419261, 0.79892127, 0.76623351,
                          0.32237578, 0.74371048, 0.89283081, 0.99471695, 0.59218713,
                          0.14229807, 0.26866107, 0.61418273, 0.53238885, 0.2847934 ,
                          0.33879263, 0.15419413, 0.81211755, 0.55982182, 0.33033445,
                          0.98925566, 0.21407401, 0.75933437, 0.50981508, 0.84659468,
                          0.27123332, 0.30602554, 0.78974943, 0.15961765, 0.75269879,
                          0.88404004, 0.25359787, 0.67575388, 0.10753205, 0.52492257,
                          0.2276367 , 0.57348205, 0.55631533, 0.48828726, 0.80950892,
                          0.68959411, 0.06038109, 0.3730253 , 0.44658293, 0.12323353,
                          0.90588169, 0.13484593, 0.58743073, 0.60592698, 0.67315081,
                          0.59887062, 0.3524358 , 0.47446065, 0.98078295, 0.31889862,
                          0.88225427, 0.81911728, 0.53942069, 0.6742203 , 0.73162166,
                          0.34597118, 0.70844054, 0.25029322, 0.29910746, 0.35906746,
                          0.53989701, 0.36776386, 0.04466711, 0.09399784, 0.53547235,
                          0.64757585, 0.03797524, 0.66378485, 0.34908186, 0.79601534,
                          0.10335962, 0.30468185, 0.57992791, 0.97139889, 0.40799129,
                          0.72985792, 0.65705408, 0.48913045, 0.46000752, 0.99260624,
                          0.52711571, 0.9383317 , 0.87126459, 0.16266698, 0.17428769,
                          0.11933665, 0.15703581, 0.17907467, 0.32411207, 0.56666047,
                          0.80868794, 0.49001672, 0.77590492, 0.63564239, 0.92169564,
                          0.5098178 , 0.40486284, 0.42978249, 0.4228141 , 0.33155423,
                          0.92695095, 0.40583767, 0.6635788 , 0.93854154, 0.48653097,
                          0.9305096 , 0.96009097, 0.03602708, 0.16874548, 0.4733966 ,
                          0.42363966, 0.18261131, 0.42653311, 0.48740795, 0.40008523,
                          0.35099519, 0.9641026 , 0.93447868, 0.16069199, 0.63925304,
                          0.17770585, 0.53600886, 0.72037108, 0.8454436 , 0.182311  ,
                          0.97860041, 0.41959913, 0.45109368, 0.24313804, 0.17884554,
                          0.04705525, 0.83247529, 0.89877392, 0.57362423, 0.27708354,
                          0.93649503, 0.43493419, 0.5422893 , 0.85565473, 0.86814896,
                          0.75788182, 0.02102082, 0.62643473, 0.24955471, 0.12775553,
                          0.96546452, 0.11001835, 0.82845919, 0.84811548, 0.21516077,
                          0.88871084, 0.55331041, 0.99744447, 0.70181741, 0.09492537,
                          0.18130881, 0.45487527, 0.82986703, 0.31207231, 0.08682494,
                          0.90971212, 0.40231716, 0.95428082, 0.10105085, 0.7243062 ,
                          0.87386255, 0.28549753, 0.90084605, 0.91034781, 0.44687279,
                          0.99318239, 0.58953929, 0.73074143, 0.05055378 };

    parameterVector D = { 0.146517  , 0.3707709 , 0.08328664, 0.49256865, 0.69405928,
                          0.61837293, 0.47376479, 0.11188706, 0.2228905 , 0.36340832,
                          0.0693854 , 0.10615699, 0.88785036, 0.6677967 , 0.63177135,
                          0.57125641, 0.80023305, 0.08298528, 0.91838322, 0.18315431,
                          0.89992512, 0.53899603, 0.41261589, 0.31495081, 0.83742576,
                          0.09300794, 0.82360698, 0.21493177, 0.1629844 , 0.21152065,
                          0.16961513, 0.4914438 , 0.50520605, 0.14553687, 0.26358359,
                          0.75964966, 0.65752746, 0.71537866, 0.0052241 , 0.96884752,
                          0.39184445, 0.9992278 , 0.82985355, 0.77985287, 0.82259158,
                          0.40209148, 0.65923576, 0.86734155, 0.82929213, 0.45634802,
                          0.27689889, 0.96708886, 0.1655354 , 0.89114231, 0.19536992,
                          0.33743959, 0.73300973, 0.363747  , 0.26554793, 0.61940794,
                          0.68798717, 0.31331865, 0.29169741, 0.94606427, 0.57961661,
                          0.9410199 , 0.09121453, 0.42521405, 0.67622819, 0.69165347,
                          0.84437122, 0.21535456, 0.15999728, 0.34674508, 0.86254606,
                          0.04035766, 0.19320383, 0.05731681, 0.95023339, 0.38534862,
                          0.01313969 };

    variableVector answerCauchyStress = { -468.08486373, -298.02079112, -315.35069455,
                                           285.12093132,  174.8563172 ,  186.50558111,
                                          -349.70100327, -236.00001236, -250.10967988 };

    variableVector answerMicroStress = { -970.04337968,    1.59908878, -705.45138092,
                                            5.89339453,  342.16732824,  -26.91143239,
                                         -700.06232934,  -32.80059522, -541.08302059 };

    variableVector answerHigherOrderStress = { 152.3275986 , -345.32313068,  168.48941995,  -87.55872352,
                                               207.72901358, -101.23620149,  118.78091452, -265.95494635,
                                               132.87071502,  -90.27961059,  203.28666944,  -99.52254888,
                                                51.72482788, -122.30453021,   59.79814044,  -70.00039233,
                                               157.01422352,  -79.06428707,  120.23269317, -274.19301168,
                                               132.51680884,  -69.47891023,  164.77681737,  -79.65682209,
                                                95.84907668, -208.69345804,  102.08397015 };

    variableVector resultCauchyStress, resultMicroStress, resultHigherOrderStress;

    errorOut error = micromorphicLinearElasticity::linearElasticity( deformationGradient, microDeformation,
                                                                     gradientMicroDeformation,
                                                                     A, B, C, D,
                                                                     resultCauchyStress, resultMicroStress,
                                                                     resultHigherOrderStress );

    if ( error ){
        error->print();
        results << "test_linearElasticity & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultCauchyStress, answerCauchyStress, 1e-5 ) ){
        results << "test_linearElasticity (test 1) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultMicroStress, answerMicroStress, 1e-5 ) ){
        results << "test_linearElasticity (test 2) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultHigherOrderStress, answerHigherOrderStress, 1e-5 ) ){
        results << "test_linearElasticity (test 3) & False\n";
        return 1;
    }

    //Test the Jacobians

    variableVector resultJCauchyStress, resultJMicroStress, resultJHigherOrderStress;

    variableMatrix dCauchyStressdF, dCauchyStressdXi, dCauchyStressdGradXi;
    variableMatrix dMicroStressdF, dMicroStressdXi, dMicroStressdGradXi;
    variableMatrix dHigherOrderStressdF, dHigherOrderStressdXi, dHigherOrderStressdGradXi;

    error = micromorphicLinearElasticity::linearElasticity( deformationGradient, microDeformation,
                                                            gradientMicroDeformation,
                                                            A, B, C, D,
                                                            resultJCauchyStress, resultJMicroStress, resultJHigherOrderStress,
                                                            dCauchyStressdF, dCauchyStressdXi, dCauchyStressdGradXi,
                                                            dMicroStressdF, dMicroStressdXi, dMicroStressdGradXi,
                                                            dHigherOrderStressdF, dHigherOrderStressdXi, dHigherOrderStressdGradXi );

    if ( error ){
        error->print();
        results << "test_mapStressesToCurrent & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJCauchyStress, answerCauchyStress, 1e-5 ) ){
        results << "test_linearElasticity (test 4) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJMicroStress, answerMicroStress, 1e-5 ) ){
        results << "test_linearElasticity (test 5) & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( resultJHigherOrderStress, answerHigherOrderStress, 1e-5 ) ){
        results << "test_linearElasticity (test 6) & False\n";
        return 1;
    }

    //Test the Jacobians w.r.t. the deformation gradient
    constantType eps = 1e-6;
    for ( unsigned int i = 0; i < deformationGradient.size(); i++ ){
        constantVector delta( deformationGradient.size(), 0 );
        delta[i] = eps * fabs( deformationGradient[i] ) + eps;

        error = micromorphicLinearElasticity::linearElasticity( deformationGradient + delta, microDeformation,
                                                                gradientMicroDeformation,
                                                                A, B, C, D,
                                                                resultJCauchyStress, resultJMicroStress,
                                                                resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_linearElasticity & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dCauchyStressdF[j][i], 1e-5 ) ){
                results << "test_linearElasticity (test 7) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMicroStressdF[j][i], 1e-5 ) ){
                results << "test_linearElasticity (test 8) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dHigherOrderStressdF[j][i], 1e-4 ) ){
                results << "test_linearElasticity (test 9) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the micro-deformation
    for ( unsigned int i = 0; i < microDeformation.size(); i++ ){
        constantVector delta( microDeformation.size(), 0 );
        delta[i] = eps * fabs( microDeformation[i] ) + eps;

        error = micromorphicLinearElasticity::linearElasticity( deformationGradient, microDeformation + delta,
                                                                gradientMicroDeformation,
                                                                A, B, C, D,
                                                                resultJCauchyStress, resultJMicroStress,
                                                                resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_linearElasticity & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dCauchyStressdXi[j][i], 1e-4, 1e-4 ) ){
                results << "test_linearElasticity (test 10) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMicroStressdXi[j][i], 1e-4, 1e-4 ) ){
                results << "test_linearElasticity (test 11) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dHigherOrderStressdXi[j][i], 1e-5 ) ){
                results << "test_linearElasticity (test 12) & False\n";
                return 1;
            }
        }
    }

    //Test Jacobians w.r.t. the gradient of the micro-deformation
    for ( unsigned int i = 0; i < gradientMicroDeformation.size(); i++ ){
        constantVector delta( gradientMicroDeformation.size(), 0 );
        delta[i] = eps * fabs( gradientMicroDeformation[i] ) + eps;

        error = micromorphicLinearElasticity::linearElasticity( deformationGradient, microDeformation,
                                                                gradientMicroDeformation + delta,
                                                                A, B, C, D,
                                                                resultJCauchyStress, resultJMicroStress,
                                                                resultJHigherOrderStress );

        if ( error ){
            error->print();
            results << "test_linearElasticity & False\n";
            return 1;
        }

        //Test Cauchy Stress
        constantVector gradCol = ( resultJCauchyStress - resultCauchyStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dCauchyStressdGradXi[j][i], 1e-5 ) ){
                results << "test_linearElasticity (test 13) & False\n";
                return 1;
            }
        }

        //Test symmetric micro stress
        gradCol = ( resultJMicroStress - resultMicroStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dMicroStressdGradXi[j][i], 1e-5 ) ){
                results << "test_linearElasticity (test 14) & False\n";
                return 1;
            }
        }

        //Test higher order stress
        gradCol = ( resultJHigherOrderStress - resultHigherOrderStress ) / delta[i];

        for ( unsigned int j = 0; j < gradCol.size(); j++ ){
            if ( !vectorTools::fuzzyEquals( gradCol[j], dHigherOrderStressdGradXi[j][i], 1e-5 ) ){
                results << "test_linearElasticity (test 15) & False\n";
                return 1;
            }
        }
    }

    results << "test_linearElasticity & True\n";
    return 0;
}

int test_formIsotropicA( std::ofstream &results){
    /*!
     * Test the formation of the isotropic A stiffness tensor.
     *
     * :param std::ofstream &results: The output file.
     */

    parameterType lambda = 4;
    parameterType mu = 7;

    parameterVector answer = { 18.,  0.,  0.,  0.,  4.,  0.,  0.,  0.,  4.,  0.,  7.,  0.,  7.,
                                0.,  0.,  0.,  0.,  0.,  0.,  0.,  7.,  0.,  0.,  0.,  7.,  0.,
                                0.,  0.,  7.,  0.,  7.,  0.,  0.,  0.,  0.,  0.,  4.,  0.,  0.,
                                0., 18.,  0.,  0.,  0.,  4.,  0.,  0.,  0.,  0.,  0.,  7.,  0.,
                                7.,  0.,  0.,  0.,  7.,  0.,  0.,  0.,  7.,  0.,  0.,  0.,  0.,
                                0.,  0.,  0.,  7.,  0.,  7.,  0.,  4.,  0.,  0.,  0.,  4.,  0.,
                                0.,  0., 18. };

    parameterVector result;

    errorOut error = micromorphicLinearElasticity::formIsotropicA( lambda, mu, result );

    if ( error ){
        error->print();
        results << "test_formIsotropicA & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( answer, result ) ){
        results << "test_formIsotropicA (test 1) & False\n";
        return 1;
    }

    results << "test_formIsotropicA & True\n";
    return 0;
}

int test_formIsotropicB( std::ofstream &results){
    /*!
     * Test the formation of the isotropic B stiffness tensor.
     *
     * :param std::ofstream &results: The output file.
     */

    parameterType eta = 3;
    parameterType tau = 5;
    parameterType kappa = 6;
    parameterType nu = 8;
    parameterType sigma = 4;

    parameterVector answer = {  4.,  0.,  0.,  0., -2.,  0.,  0.,  0., -2.,  0.,  2.,  0.,  4.,
                                0.,  0.,  0.,  0.,  0.,  0.,  0.,  2.,  0.,  0.,  0.,  4.,  0.,
                                0.,  0.,  4.,  0.,  2.,  0.,  0.,  0.,  0.,  0., -2.,  0.,  0.,
                                0.,  4.,  0.,  0.,  0., -2.,  0.,  0.,  0.,  0.,  0.,  2.,  0.,
                                4.,  0.,  0.,  0.,  4.,  0.,  0.,  0.,  2.,  0.,  0.,  0.,  0.,
                                0.,  0.,  0.,  4.,  0.,  2.,  0., -2.,  0.,  0.,  0., -2.,  0.,
                                0.,  0.,  4. };

    parameterVector result;

    errorOut error = micromorphicLinearElasticity::formIsotropicB( eta, tau, kappa, nu, sigma, result );

    if ( error ){
        error->print();
        results << "test_formIsotropicB & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( answer, result ) ){
        results << "test_formIsotropicB (test 1) & False\n";
        return 1;
    }

    results << "test_formIsotropicB & True\n";
    return 0;
}

int test_formIsotropicC( std::ofstream &results){
    /*!
     * Test the formation of the isotropic C stiffness tensor.
     *
     * :param std::ofstream &results: The output file.
     */

    parameterVector taus = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

    parameterVector answer = {
        97.,  0.,  0.,  0., 13.,  0.,  0.,  0., 13.,  0., 16.,  0.,  9.,
        0.,  0.,  0.,  0.,  0.,  0.,  0., 16.,  0.,  0.,  0.,  9.,  0.,
        0.,  0., 23.,  0., 22.,  0.,  0.,  0.,  0.,  0., 23.,  0.,  0.,
        0.,  9.,  0.,  0.,  0.,  2.,  0.,  0.,  0.,  0.,  0.,  3.,  0.,
        4.,  0.,  0.,  0., 23.,  0.,  0.,  0., 22.,  0.,  0.,  0.,  0.,
        0.,  0.,  0.,  4.,  0.,  3.,  0., 23.,  0.,  0.,  0.,  2.,  0.,
        0.,  0.,  9.,  0., 22.,  0., 27.,  0.,  0.,  0.,  0.,  0., 26.,
        0.,  0.,  0., 16.,  0.,  0.,  0.,  6.,  0.,  0.,  0.,  0.,  0.,
        7.,  0.,  3.,  0., 13.,  0.,  0.,  0., 23.,  0.,  0.,  0.,  5.,
        0., 26.,  0., 23.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  6.,  0.,
        0.,  0.,  2.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  8.,  0., 10.,
        0.,  0.,  0., 11.,  0.,  0.,  0.,  9.,  0.,  0.,  0.,  9.,  0.,
       12.,  0.,  0.,  0.,  0.,  0.,  0.,  0., 22.,  0.,  0.,  0., 27.,
        0.,  0.,  0.,  0.,  0.,  0.,  0.,  3.,  0.,  7.,  0., 26.,  0.,
        0.,  0.,  6.,  0.,  0.,  0., 16.,  0.,  0.,  0.,  0.,  0., 10.,
        0.,  8.,  0.,  0.,  0.,  9.,  0.,  0.,  0., 12.,  0.,  0.,  0.,
       11.,  0.,  9.,  0.,  0.,  0.,  0.,  0., 13.,  0.,  0.,  0.,  5.,
        0.,  0.,  0., 23.,  0.,  6.,  0.,  2.,  0.,  0.,  0.,  0.,  0.,
        0.,  0., 26.,  0.,  0.,  0., 23.,  0.,  0.,  0., 23.,  0., 26.,
        0.,  0.,  0.,  0.,  0., 23.,  0.,  0.,  0., 13.,  0.,  0.,  0.,
        5.,  0.,  0.,  0.,  0.,  0.,  6.,  0.,  2.,  0., 16.,  0.,  0.,
        0., 26.,  0.,  0.,  0.,  6.,  0., 27.,  0., 22.,  0.,  0.,  0.,
        0.,  0.,  0.,  0.,  7.,  0.,  0.,  0.,  3.,  0.,  0.,  0.,  0.,
        0.,  0.,  0., 11.,  0.,  9.,  0.,  0.,  0.,  8.,  0.,  0.,  0.,
       10.,  0.,  0.,  0., 12.,  0.,  9.,  0.,  0.,  0.,  0.,  0.,  9.,
        0.,  0.,  0., 23.,  0.,  0.,  0.,  2.,  0., 22.,  0., 23.,  0.,
        0.,  0.,  0.,  0.,  0.,  0.,  3.,  0.,  0.,  0.,  4.,  0.,  0.,
        0.,  9.,  0., 16.,  0.,  0.,  0.,  0.,  0., 13.,  0.,  0.,  0.,
       97.,  0.,  0.,  0., 13.,  0.,  0.,  0.,  0.,  0., 16.,  0.,  9.,
        0.,  0.,  0.,  4.,  0.,  0.,  0.,  3.,  0.,  0.,  0.,  0.,  0.,
        0.,  0., 23.,  0., 22.,  0.,  2.,  0.,  0.,  0., 23.,  0.,  0.,
        0.,  9.,  0.,  0.,  0.,  0.,  0.,  9.,  0., 12.,  0.,  0.,  0.,
       10.,  0.,  0.,  0.,  8.,  0.,  0.,  0.,  9.,  0., 11.,  0.,  0.,
        0.,  0.,  0.,  0.,  0.,  3.,  0.,  0.,  0.,  7.,  0.,  0.,  0.,
        0.,  0.,  0.,  0., 22.,  0., 27.,  0.,  6.,  0.,  0.,  0., 26.,
        0.,  0.,  0., 16.,  0.,  2.,  0.,  6.,  0.,  0.,  0.,  0.,  0.,
        5.,  0.,  0.,  0., 13.,  0.,  0.,  0., 23.,  0.,  0.,  0.,  0.,
        0., 26.,  0., 23.,  0.,  0.,  0., 23.,  0.,  0.,  0., 26.,  0.,
        0.,  0.,  0.,  0.,  0.,  0.,  2.,  0.,  6.,  0., 23.,  0.,  0.,
        0.,  5.,  0.,  0.,  0., 13.,  0.,  0.,  0.,  0.,  0.,  9.,  0.,
       11.,  0.,  0.,  0., 12.,  0.,  0.,  0.,  9.,  0.,  0.,  0.,  8.,
        0., 10.,  0.,  0.,  0.,  0.,  0., 16.,  0.,  0.,  0.,  6.,  0.,
        0.,  0., 26.,  0.,  7.,  0.,  3.,  0.,  0.,  0.,  0.,  0.,  0.,
        0., 27.,  0.,  0.,  0., 22.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
       12.,  0.,  9.,  0.,  0.,  0.,  9.,  0.,  0.,  0., 11.,  0.,  0.,
        0., 10.,  0.,  8.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  2.,  0.,
        0.,  0.,  6.,  0.,  0.,  0.,  0.,  0.,  0.,  0., 23.,  0., 26.,
        0.,  5.,  0.,  0.,  0., 23.,  0.,  0.,  0., 13.,  0.,  3.,  0.,
        7.,  0.,  0.,  0.,  0.,  0.,  6.,  0.,  0.,  0., 16.,  0.,  0.,
        0., 26.,  0.,  0.,  0.,  0.,  0., 27.,  0., 22.,  0.,  9.,  0.,
        0.,  0.,  2.,  0.,  0.,  0., 23.,  0.,  3.,  0.,  4.,  0.,  0.,
        0.,  0.,  0.,  0.,  0., 22.,  0.,  0.,  0., 23.,  0.,  0.,  0.,
        4.,  0.,  3.,  0.,  0.,  0.,  0.,  0.,  2.,  0.,  0.,  0.,  9.,
        0.,  0.,  0., 23.,  0.,  0.,  0.,  0.,  0., 22.,  0., 23.,  0.,
        0.,  0.,  9.,  0.,  0.,  0., 16.,  0.,  0.,  0.,  0.,  0.,  0.,
        0.,  9.,  0., 16.,  0., 13.,  0.,  0.,  0., 13.,  0.,  0.,  0.,
       97.
    };

    parameterVector result;

    errorOut error = micromorphicLinearElasticity::formIsotropicC( taus, result );

    if ( error ){
        error->print();
        results << "test_formIsotropicC & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( answer, result ) ){
        results << "test_formIsotropicC (test 1) & False\n";
        return 1;
    }

    results << "test_formIsotropicC & True\n";
    return 0;
}

int test_formIsotropicD( std::ofstream &results){
    /*!
     * Test the formation of the isotropic D stiffness tensor.
     *
     * :param std::ofstream &results: The output file.
     */

    parameterType tau = 5;
    parameterType sigma = 4;

    parameterVector answer = { 13.,  0.,  0.,  0.,  5.,  0.,  0.,  0.,  5.,  0.,  4.,  0.,  4.,
                                0.,  0.,  0.,  0.,  0.,  0.,  0.,  4.,  0.,  0.,  0.,  4.,  0.,
                                0.,  0.,  4.,  0.,  4.,  0.,  0.,  0.,  0.,  0.,  5.,  0.,  0.,
                                0., 13.,  0.,  0.,  0.,  5.,  0.,  0.,  0.,  0.,  0.,  4.,  0.,
                                4.,  0.,  0.,  0.,  4.,  0.,  0.,  0.,  4.,  0.,  0.,  0.,  0.,
                                0.,  0.,  0.,  4.,  0.,  4.,  0.,  5.,  0.,  0.,  0.,  5.,  0.,
                                0.,  0., 13. };

    parameterVector result;

    errorOut error = micromorphicLinearElasticity::formIsotropicD( tau, sigma, result );

    if ( error ){
        error->print();
        results << "test_formIsotropicD & False\n";
        return 1;
    }

    if ( !vectorTools::fuzzyEquals( answer, result ) ){
        results << "test_formIsotropicD (test 1) & False\n";
        return 1;
    }

    results << "test_formIsotropicD & True\n";
    return 0;
}

int main(){
    /*!
    The main loop which runs the tests defined in the 
    accompanying functions. Each function should output
    the function name followed by & followed by True or False 
    if the test passes or fails respectively.
    */

    //Open the results file
    std::ofstream results;
    results.open("results.tex");

    //Run the tests
    test_computeDeformationMeasures( results );
    test_computeLinearElasticTerm1( results );
    test_computeLinearElasticTerm2( results );
    test_computeLinearElasticTerm3( results );
    test_computeReferenceHigherOrderStress( results );
    test_computeInvRCGPsi( results );
    test_computeInvRCGGamma( results );
    test_linearElasticityReference( results );
    test_linearElasticityReferenceDerivedMeasures( results );
    test_mapStressesToCurrent( results );
    test_linearElasticity( results );

    test_formIsotropicA( results );
    test_formIsotropicB( results );
    test_formIsotropicC( results );
    test_formIsotropicD( results );

    //Close the results file
    results.close();

    return 0;
}
