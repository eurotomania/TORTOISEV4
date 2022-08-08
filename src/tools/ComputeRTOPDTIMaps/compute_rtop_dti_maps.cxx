

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "itkDiffusionTensor3D.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkComposeImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"


    typedef double RealType;     
    typedef itk::Image<RealType,4> ImageType4D;
    typedef itk::Image<RealType,3> ImageType3D;
    typedef itk::DiffusionTensor3D<RealType> TensorPixelType;
    typedef itk::Vector<RealType,6> VectorPixelType;
    
    typedef itk::Image<itk::DiffusionTensor3D<RealType>,3>         TensorImageType;
    typedef itk::Image<VectorPixelType,3>         VectorImageType;
    
    typedef  vnl_matrix_fixed< RealType, 3, 3 > InternalMatrixType;
    

#define DPI 3.1415926535897931

int main( int argc , char * argv[] )
{
    if(argc<4)
    {
        std::cout<< "Computes the RTOP, RTAP and RTPP maps from the diffusion tensor model"<<std::endl;
        std::cout<<"Usage:   ComputeRTOPDTIMaps full_path_to_tensor_image small_delta big_delta"<<std::endl;
        return 0;
    }
    
    double small_delta = atof(argv[2]);
    double big_delta = atof(argv[3]);


    std::string currdir;
    std::string nm(argv[1]);
    
   
            
    int mypos= nm.rfind("/");
    if(mypos ==-1)
        currdir= std::string("./");
    else   
       currdir= nm.substr(0,mypos+1);
    

    typedef itk::ImageFileReader<ImageType4D> ImageType4DReaderType;
    ImageType4DReaderType::Pointer imager= ImageType4DReaderType::New();
    imager->SetFileName(nm);
    imager->Update();
    ImageType4D::Pointer image4D= imager->GetOutput();
    
    
    ImageType4D::SizeType imsize= image4D->GetLargestPossibleRegion().GetSize();
    ImageType4D::IndexType index;
    
    
    ImageType3D::SizeType nsize;
    nsize[0]=imsize[0];
    nsize[1]=imsize[1];
    nsize[2]=imsize[2];
    
    ImageType3D::IndexType start; start.Fill(0);
    ImageType3D::RegionType reg(start,nsize);
    ImageType3D::PointType norig;
    norig[0]=image4D->GetOrigin()[0];
    norig[1]=image4D->GetOrigin()[1];
    norig[2]=image4D->GetOrigin()[2];
    
    ImageType3D::SpacingType nspc;
    nspc[0]=image4D->GetSpacing()[0];
    nspc[1]=image4D->GetSpacing()[1];
    nspc[2]=image4D->GetSpacing()[2];
    
    ImageType3D::DirectionType ndir;
    ImageType4D::DirectionType dir = image4D->GetDirection();
    ndir(0,0)=dir(0,0);ndir(0,1)=dir(0,1);ndir(0,2)=dir(0,2);
    ndir(1,0)=dir(1,0);ndir(1,1)=dir(1,1);ndir(1,2)=dir(1,2);
    ndir(2,0)=dir(2,0);ndir(2,1)=dir(2,1);ndir(2,2)=dir(2,2);          
    
    
    


    ImageType3D::Pointer rtop_image = ImageType3D::New();
    rtop_image->SetRegions(reg);
    rtop_image->Allocate();
    rtop_image->SetOrigin(norig);
    rtop_image->SetSpacing(nspc);
    rtop_image->SetDirection(ndir);
    rtop_image->FillBuffer(0.);

    ImageType3D::Pointer rtap_image = ImageType3D::New();
    rtap_image->SetRegions(reg);
    rtap_image->Allocate();
    rtap_image->SetOrigin(norig);
    rtap_image->SetSpacing(nspc);
    rtap_image->SetDirection(ndir);
    rtap_image->FillBuffer(0.);

    ImageType3D::Pointer rtpp_image = ImageType3D::New();
    rtpp_image->SetRegions(reg);
    rtpp_image->Allocate();
    rtpp_image->SetOrigin(norig);
    rtpp_image->SetSpacing(nspc);
    rtpp_image->SetDirection(ndir);
    rtpp_image->FillBuffer(0.);

    ImageType3D::IndexType ind;

    InternalMatrixType id;
    id.set_identity();
    

    double tdiff= big_delta-small_delta/3.;
    double uvec[3],uu[3];


    for(int k=0;k<imsize[2];k++)
    {
        index[2]=k;        
        ind[2]=k;
        for(int j=0;j<imsize[1];j++)
        {
            index[1]=j;            
            ind[1]=j;
            for(int i=0;i<imsize[0];i++)
            {
                index[0]=i;    
                ind[0]=i;
                
                InternalMatrixType curr_tens;
                
                index[3]=0;
                curr_tens(0,0)=image4D->GetPixel(index);
                index[3]=1;
                curr_tens(1,1)=image4D->GetPixel(index);
                index[3]=2;
                curr_tens(2,2)=image4D->GetPixel(index);
                index[3]=3;
                curr_tens(0,1)=image4D->GetPixel(index);
                curr_tens(1,0)=image4D->GetPixel(index);
                index[3]=4;
                curr_tens(0,2)=image4D->GetPixel(index);
                curr_tens(2,0)=image4D->GetPixel(index);
                index[3]=5;
                curr_tens(1,2)=image4D->GetPixel(index);
                curr_tens(2,1)=image4D->GetPixel(index);


                if( (curr_tens(0,0)!=curr_tens(0,0)) ||
                    (curr_tens(0,1)!=curr_tens(0,1)) ||
                    (curr_tens(0,2)!=curr_tens(0,2)) ||
                    (curr_tens(1,1)!=curr_tens(1,1)) ||
                    (curr_tens(1,2)!=curr_tens(1,2)) ||
                    (curr_tens(2,2)!=curr_tens(2,2)))
                    continue;

                if(curr_tens(0,0)+curr_tens(1,1)+curr_tens(2,2)  <1)
                    continue;

                curr_tens= curr_tens/1000000.;
                
                vnl_symmetric_eigensystem<double>  eig(curr_tens);

                uvec[0]=eig.D(2,2);
                uvec[1]=eig.D(1,1);
                uvec[2]=eig.D(0,0);

                uu[0]= sqrt(uvec[0]*2000.*tdiff);
                uu[1]= sqrt(uvec[1]*2000.*tdiff);
                uu[2]= sqrt(uvec[2]*2000.*tdiff);

                double rtop =   1./sqrt(8.*DPI*DPI*DPI * uu[0]*uu[0]*uu[1]*uu[1]*uu[2]*uu[2]);

                double rtap =  1./ (2*DPI* uu[1]*uu[2]);

                double rtpp =   1./(sqrt(2*DPI)*uu[0]);

                rtop_image->SetPixel(ind,rtop);
                rtap_image->SetPixel(ind,rtap);
                rtpp_image->SetPixel(ind,rtpp);
            }   
        }   
    }
    
       
    
       
    {
    std::string filename(argv[1]);
    std::string::size_type idx=filename.rfind('.');
    std::string basename= filename.substr(mypos+1,idx-mypos-1);
    std::string output_name=currdir + basename + std::string("_RTOPDTI.nii");
       
    
    
    typedef itk::ImageFileWriter<ImageType3D> WriterType;
    WriterType::Pointer writer= WriterType::New();
    writer->SetFileName(output_name);
    writer->SetInput(rtop_image);
    writer->Update();
    }
                    
    {
    std::string filename(argv[1]);
    std::string::size_type idx=filename.rfind('.');
    std::string basename= filename.substr(mypos+1,idx-mypos-1);
    std::string output_name=currdir + basename + std::string("_RTAPDTI.nii");



    typedef itk::ImageFileWriter<ImageType3D> WriterType;
    WriterType::Pointer writer= WriterType::New();
    writer->SetFileName(output_name);
    writer->SetInput(rtap_image);
    writer->Update();
    }

    {
    std::string filename(argv[1]);
    std::string::size_type idx=filename.rfind('.');
    std::string basename= filename.substr(mypos+1,idx-mypos-1);
    std::string output_name=currdir + basename + std::string("_RTPPDTI.nii");



    typedef itk::ImageFileWriter<ImageType3D> WriterType;
    WriterType::Pointer writer= WriterType::New();
    writer->SetFileName(output_name);
    writer->SetInput(rtpp_image);
    writer->Update();
    }


    

    
    return EXIT_SUCCESS;
}
