//libreria di funzioni che ricavano info leggendo le pshapes
#include <sstream>
//BASELINE
float GetBaseline(const float* pshape, int npoints){
  float baseline = 0;
  int   i_min    = 0;
  int   i_max    = 450; 		     

  for(int i=i_min; i<i_max; i++){
    baseline += pshape[i];
  }
  baseline /= (i_max-i_min);
  return baseline;
}


//STD_DEV BASELINE
float GetBaselineError(const float* pshape, int npoints, float baseline){
  float baseline_error = 0;
  int   i_min          = 0;
  int   i_max          = 450;	

  for(int i=i_min; i<i_max; i++){
    baseline_error += pow((pshape[i]-baseline),2);
  }
  baseline_error /= (i_max-i_min-1);
  return sqrt(baseline_error);
}


//CHARGE
float GetCharge(const float* pshape, int npoints, float dt, float baseline){
  float charge = 0;
  
  for(int i=0; i<npoints; i++){
    charge += (pshape[i]-baseline);
  }
  
  charge *= dt;
  return (charge*(-1)); //lavoro con carica>0
}


//AMPLITUDE
float GetAmp(const float* pshape, int npoints, float baseline){
  float amp = 0;
  float min = 100;
  
  //sto lavorando sempre con segnali ingiù
  for(int i=0; i<npoints; i++){
    if (pshape[i]<min){
      min = pshape[i];
    }
  }
  amp = baseline - min; //ampiezza>0
  return (amp);
}



void DynamicMean(int npoints, float* delta, float* dyn_mean, int j_max){
  //preparazione array media dinamica
  //se qualcuno non si fida, provi l'algoritmo a parte
  //spiegarlo è un macello :')
  //grazie per la fiducia

  std::vector<int> counter(npoints);
  std::vector<int> index(npoints, 0);

  for(int l=0; l<npoints; l++){
    counter[l] = int(j_max/2);

    if(l < (int(j_max/2)+1)){
      counter[l] -= (int(j_max/2)-l);
      index[l]   += (int(j_max/2)-l);
    }

    if(l > (npoints-int(j_max/2)-1)){
      counter[l] -= (int(j_max/2)+l-(npoints-1));
    }
  }

  //leggo tutta la pshape
  for(int i=0; i<npoints; i++){

    for(int j=0; j<(int(j_max/2)+counter[i]+1); j++){
      dyn_mean[i] += delta[j+i-int(j_max/2)+index[i]];
    }

    dyn_mean[i] /= (int(j_max/2)+1+counter[i]);
  }
}



void search_and_remove(const std::filesystem::path& directory, const std::filesystem::path& file_name){
     std::stringstream stream;  
     auto d = std::filesystem::directory_iterator(directory);

    auto found = std::find_if(d, end(d), [&file_name](const auto& dir_entry)
    {
        return dir_entry.path().filename() == file_name;
    });

    if (found != end(d))
    {
        //elimino tutti i tree singoli inutili
      stream << "cd " << directory.c_str() << std::endl;
      stream << "rm -f " << file_name.c_str() << std::endl;
      system(stream.str().c_str());
      
    }
}  



