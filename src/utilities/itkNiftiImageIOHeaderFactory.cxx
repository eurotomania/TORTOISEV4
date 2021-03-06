/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkNiftiImageIOHeaderFactory.h"
#include "itkNiftiImageIOHeader.h"
#include "itkVersion.h"

namespace itk
{
void NiftiImageIOHeaderFactory::PrintSelf(std::ostream &, Indent) const
{}

NiftiImageIOHeaderFactory::NiftiImageIOHeaderFactory()
{
  this->RegisterOverride( "itkImageIOBase",
                          "itkNiftiImageIOHeader",
                          "Nifti Image IO",
                          1,
                          CreateObjectFunction< NiftiImageIOHeader >::New() );
}

NiftiImageIOHeaderFactory::~NiftiImageIOHeaderFactory()
{}

const char *
NiftiImageIOHeaderFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char *
NiftiImageIOHeaderFactory::GetDescription() const
{
  return "Nifti ImageIO Factory, allows the loading of Nifti images into insight";
}

// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.

static bool NiftiImageIOHeaderFactoryHasBeenRegistered;

void NiftiImageIOHeaderFactoryRegister__Private(void)
{
  if( ! NiftiImageIOHeaderFactoryHasBeenRegistered )
    {
    NiftiImageIOHeaderFactoryHasBeenRegistered = true;
    NiftiImageIOHeaderFactory::RegisterOneFactory();
    }
}

} // end namespace itk
