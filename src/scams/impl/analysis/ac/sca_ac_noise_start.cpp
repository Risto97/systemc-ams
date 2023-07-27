/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
    COSEDA Technologies GmbH

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sca_ac_noise_start.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2045 $
   SVN last checkin  :  $Date: 2017-09-19 14:05:56 +0000 (Tue, 19 Sep 2017) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_noise_start.cpp 2045 2017-09-19 14:05:56Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/analysis/ac/sca_ac_noise_start.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/analysis/ac/sca_ac_domain_solver.h"


namespace sca_ac_analysis
{

//////////////////////////////////////////////////////////////////////////////
void sca_ac_noise_start(const sca_util::sca_vector<double>& frequencies)
{
    std::vector<double> omegas;
    for(unsigned long i=0;i<frequencies.length();i++)
        omegas.push_back(2.0*M_PI*frequencies(i));


    sca_ac_analysis::sca_implementation::sca_ac_domain_solver
							ac_solver(sca_ac_analysis::sca_implementation::get_ac_database(),true);
    ac_solver.calculate_noise(omegas);
}


//////////////////////////////////////////////////////////////////////////////

void sca_ac_noise_start(double start_freq, double stop_freq, unsigned long npoints,
		sca_ac_analysis::sca_ac_scale scale)
{
    sca_util::sca_vector<double> freqs;
    sca_ac_noise_start(
    		sca_ac_analysis::sca_implementation::generate_frequencies(
    				freqs,
    				start_freq,
    				stop_freq,
    				npoints,
    				scale));
}




} // namespace sca_ac_analysis

