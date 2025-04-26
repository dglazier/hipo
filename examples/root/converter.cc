//******************************************************************
//*  ██╗  ██╗██╗██████╗  ██████╗     ██╗  ██╗    ██████╗
//*  ██║  ██║██║██╔══██╗██╔═══██╗    ██║  ██║   ██╔═████╗
//*  ███████║██║██████╔╝██║   ██║    ███████║   ██║██╔██║
//*  ██╔══██║██║██╔═══╝ ██║   ██║    ╚════██║   ████╔╝██║
//*  ██║  ██║██║██║     ╚██████╔╝         ██║██╗╚██████╔╝
//*  ╚═╝  ╚═╝╚═╝╚═╝      ╚═════╝          ╚═╝╚═╝ ╚═════╝
//************************ Jefferson National Lab (2017) ***********
//******************************************************************
//* Example program for reading HIPO-4 Files..
//* Reads the file and converts the file to ROOT.
//*--
//* Author: G.Gavalian
//*

#include <cstdlib>
#include <iostream>
#include "reader.h"
#include "writer.h"
#include "utils.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "Compression.h"


void    convert(const char *hipoFile);
double  benchmarkRoot(const char *file);
double  benchmarkHipo(const char *file);

int main(int argc, char** argv) {

   std::cout << " reading file example program (HIPO) "
   << __cplusplus << std::endl;

   char inputFile[512];
   char outputFile[512];
   char outputFileHipo[512];

   if(argc>1) {
      sprintf(inputFile,"%s",argv[1]);
      sprintf(outputFile,"%s.root",argv[1]);
      sprintf(outputFileHipo,"%s_writer.hipo",argv[1]);
      //sprintf(outputFile,"%s",argv[2]);
   } else {
      std::cout << " *** please provide a file name..." << std::endl;
     exit(1);
   }

   convert(inputFile);
}

void convert(const char *hipoFile){

   int compression = 404;

   char outputFile[512];
   char outputFileHipo[512];
   sprintf(outputFile,"%s_w.root",hipoFile);
   sprintf(outputFileHipo,"%s_w.hipo",hipoFile);

   auto fileOutput = new TFile(outputFile, "RECREATE");
   fileOutput->SetCompressionSettings(compression);
   //fileOutput->SetCompressionLevel(ROOT::RCompressionSetting::ELevel::kDefaultLZ4);
   ROOT::TIOFeatures features;
   features.Set(ROOT::Experimental::EIOFeatures::kGenerateOffsetMap);
   auto treeOutput = new TTree("clas12", "");
   
   //if (iofeatures)
   //   treeOutput->SetIOFeatures(features);

   int   count;
   int   outPid[1000];
   float outPx[1000];
   float outPy[1000];
   float outPz[1000];
   float outVx[1000];
   float outVy[1000];
   float outVz[1000];
   float outVt[1000];
   float outBeta[1000];
   float outChi2pid[1000];
   short  outCharge[1000];
   short outStatus[1000];
   treeOutput->Branch("count",   &count,     "count/I",          512000);
   treeOutput->Branch("pid",     outPid,     "pid[count]/I",     512000);
   treeOutput->Branch("px",      outPx,      "px[count]/F",      512000);
   treeOutput->Branch("py",      outPy,      "py[count]/F",      512000);
   treeOutput->Branch("pz",      outPz,      "pz[count]/F",      512000);
   treeOutput->Branch("vx",      outVx,      "vx[count]/F",      512000);
   treeOutput->Branch("vy",      outVy,      "vy[count]/F",      512000);
   treeOutput->Branch("vz",      outVz,      "vz[count]/F",      512000);
   treeOutput->Branch("vt",      outVt,      "vt[count]/F",      512000);
   treeOutput->Branch("beta",    outBeta,    "beta[count]/F",    512000);
   treeOutput->Branch("chi2pid", outChi2pid, "chi2pid[count]/F", 512000);
   treeOutput->Branch("charge",  outCharge,  "charge[count]/S",  512000);
   treeOutput->Branch("status",  outStatus,  "status[count]/S",  512000);

   int nEv = 0;
   long runtime = 0;

   hipo::reader  reader;
   reader.open(hipoFile);
   hipo::dictionary  factory;
   reader.readDictionary(factory);

   factory.show();

   hipo::writer writer ;
   writer.getDictionary().addSchema(factory.getSchema("REC::Particle"));
   writer.open(outputFileHipo);

   hipo::bank  pt(factory.getSchema("REC::Particle"));

   hipo::event      event;

   int counter = 0;

   hipo::benchmark     writerBenchmark;
   hipo::benchmark     readerBenchmark;
   hipo::benchmark   transferBenchmark;
   hipo::benchmark       restBenchmark;
   hipo::benchmark       writerHipoBenchmark;


   while(reader.next()==true){

      readerBenchmark.resume();
      reader.read(event);
      event.getStructure(pt);
      readerBenchmark.pause();

      count = pt.getRows();
      assert(count < 1000);
      for (int i = 0; i < count; ++i) {
         outPid[i]      = pt.getInt("pid",i);
         outPx[i]       = pt.getFloat("px",i);
         outPy[i]       = pt.getFloat("py",i);
         outPz[i]       = pt.getFloat("pz",i);
         outVx[i]       = pt.getFloat("vx",i);
         outVy[i]       = pt.getFloat("vy",i);
         outVz[i]       = pt.getFloat("vz",i);
         outVt[i]       = pt.getFloat("vt",i);
         outBeta[i]     = pt.getFloat("beta",i);
         outChi2pid[i]  = pt.getFloat("chi2pid",i);
         outCharge[i]   = (short) pt.getInt("charge",i);
         outStatus[i]   = pt.getShort("status",i);
	 //std::cout << " charge = " << outCharge[i] << " ::: " << charge->at(i)  << std::endl;
      }

      if(count>0){
          writerBenchmark.resume();
          treeOutput->Fill();
          writerBenchmark.pause();

         writerHipoBenchmark.resume();
         event.reset();
         event.addStructure(pt);
         writer.addEvent(event);
         writerHipoBenchmark.pause();
      }
  }
  writer.close();
  fileOutput->Write();
  fileOutput->Close();

  printf("processed events = %d, root      (WRITE) : time = %10.2f sec , count = %d\n",
      counter,writerBenchmark.getTimeSec(),writerBenchmark.getCounter());
   printf("processed events = %d, hipo      (WRITE) : time = %10.2f sec , count = %d\n",
         counter,writerHipoBenchmark.getTimeSec(),writerHipoBenchmark.getCounter());
   printf("processed events = %d, benchmark (READ)  : time = %10.2f sec , count = %d\n",
      counter,readerBenchmark.getTimeSec(),readerBenchmark.getCounter());
   printf("processed events = %d, benchmark (COPY)  : time = %10.2f sec , count = %d\n",
      counter,transferBenchmark.getTimeSec(),transferBenchmark.getCounter());
      printf("processed events = %d, benchmark (REST)  : time = %10.2f sec , count = %d\n",
         counter,restBenchmark.getTimeSec(),restBenchmark.getCounter());

   double total_time = writerBenchmark.getTimeSec() + readerBenchmark.getTimeSec()
          + transferBenchmark.getTimeSec() + restBenchmark.getTimeSec();
    printf("\n total time = %10.2f\n",total_time);

    printf("[FIN] (C++ ) hipo write : %10.4f sec\n",writerHipoBenchmark.getTimeSec());
    printf("[FIN] (C++ ) root write : %10.4f sec\n",writerBenchmark.getTimeSec());
}

//### END OF GENERATED CODE
