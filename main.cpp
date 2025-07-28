#include "includes.h"
#include "globales.h"
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

bool debug=true;
bool debug2=false;

int int_rand (int a, int b)
{ 
  int retorno = 0;
  if (a < b)
  {
    retorno = (int) ((b - a) * drand48 ());
    retorno = retorno + a;
  }
  else
  {
    retorno = (int) ((a - b) * drand48 ());
    retorno = retorno + b;
  }
  return retorno;
}


void salir(void)
{ 
  FILE *archivo;
  char *archivo_parametros="EVOCA.params";
  if(debug) printf("Parameters file: %s\n", archivo_parametros);
  archivo = fopen (archivo_parametros, "w");
  fprintf(archivo,"%s ", archivo_instancias);
   
  for(int p=0; p<cantidad_parametros; p++){
    if(configuracion_parametros[p].decimales > 1)
      fprintf(archivo," %.5f ", poblacion.cjto[0].parametro[p]/(float)configuracion_parametros[p].decimales);
    else
      fprintf(archivo, "%d ", poblacion.cjto[0].parametro[p]);
  }
  fprintf(archivo,"\n");
  fclose(archivo);

  exit(0);
  return;
}

void agregar_semilla_instancia(vector <si> *lista)
{
  si si_temp;
  ifstream infile;
  infile.open(archivo_instancias);
  int sel=int_rand(0, CIT);
  int c=0;
  if(infile){
    string s="";
    while(infile){
      getline(infile,s);
      if(sel==c)
      {
        si_temp.instance=s;
        si_temp.seed=int_rand(0,10000);
        lista_semillas_instancias.push_back(si_temp);
        infile.close();
        return;
      }
      c++;
    }
  }
  else{
    cout<<"Archivo de instancias no encontrado!";
    infile.close();
  }
  return;
}

void mostrar_lista_semillas_instancias(void){
  for (vector<si>::const_iterator l=lista_semillas_instancias.begin(); l!=lista_semillas_instancias.end(); ++l){
    int sem=l->seed;
    const char *ins = l->instance.c_str();
    cout<<"S:"<<sem<<" I:"<<ins<<endl;
  }
  return; 
}

void leer_archivo_configuracion(void){
  FILE *archivo;
  int response;
  archivo=fopen(archivo_configuracion, "r");
  response=fscanf(archivo, "%d", &cantidad_parametros);
  cout<<cantidad_parametros<<endl;

  if(cantidad_parametros < 0){
    printf("Cantidad de parametros invalida");
    fclose(archivo);
    exit(1);
  }
  
  configuracion_parametros=new( struct configuracion [cantidad_parametros] );
  
  for(int i=0; i< cantidad_parametros; i++){
    response=fscanf(archivo, "%s", configuracion_parametros[i].nombre);
    printf("nombre leido: %s\n", configuracion_parametros[i].nombre);
    response=fscanf(archivo, "%d", &configuracion_parametros[i].limite_minimo);
    printf("lim_min: %d\n", configuracion_parametros[i].limite_minimo);
    response=fscanf(archivo, "%d", &configuracion_parametros[i].limite_maximo);
    printf("lim_max: %d\n", configuracion_parametros[i].limite_maximo);
    response=fscanf(archivo, "%d", &configuracion_parametros[i].decimales);
    printf("decimales: %d\n", configuracion_parametros[i].decimales);
  }
  
  response=fscanf(archivo, "%d", &maximo_decimales);
  fclose(archivo);
  if(debug) printf("leer_archivo_configuracion done!\n");
  return;
}


void calcular_aptitud_semilla_instancia(calibracion *cal_temp, const char* instancia, int semilla, int s)
{
  float f_total=0.00;
  int response;

  ifstream resultados;
  FILE * sh;
  
  char archivo_resultados[1000];
  char archivo_sh[1000];
  strcpy (archivo_resultados, "resultados.res");
  strcpy (archivo_sh, "ejecutar_");
  strcat (archivo_sh, archivo_instancias);
  strcat (archivo_sh, ".sh");

  char comando[1024];
  char comando2[1024];
  char comando3[1024];
  float aptitud;

  for(int i=0; i< cantidad_parametros; i++)
    cout<<cal_temp->parametro[i]<<" ";
  cout<<endl;

  fflush(stdout);
  sprintf(comando, "rm -rf %s", archivo_resultados);
  response=system(comando);

  sprintf(comando, "bash %s %s %s %d", ejecutable, instancia, archivo_resultados, semilla);
  sprintf(comando3, "%s  %d", instancia, semilla);

  for(int i=0; i<cantidad_parametros; i++)
  {
    if(configuracion_parametros[i].decimales>1){
      sprintf(comando2," -%s %.4f", configuracion_parametros[i].nombre,(cal_temp->parametro[i]/((float)configuracion_parametros[i].decimales)));
    }
     
    else{
      sprintf(comando2," -%s %d", configuracion_parametros[i].nombre, (int)(cal_temp->parametro[i]));
    }
    strcat(comando, comando2);
    strcat(comando3, comando2);
    
  }

  printf("%s\n", comando);
  response=system(comando);
  resultados.open(archivo_resultados);
  resultados>>aptitud;
  resultados.close();
    
  if(MAX_TIME!=0) //Cuando MAX_TIME==0, no se considera tiempo limite
    T+=aptitud;

  cal_temp->aptitud_promedio = (cal_temp->aptitud_promedio*(s) + aptitud)/((s)+1);
  printf("Aptitud Total: %2f (%d)\n", (cal_temp->aptitud_promedio), (s));
  
  E++;
  cout<<"-->Van "<< E <<" evaluaciones ("<< T <<" segundos)"<<endl;
  if((MAX_EVAL!=0) && (E>MAX_EVAL)) // Cuando MAX_EVAL==0, no se considera numero de evaluaciones limite
  {
    cout<<"MAX_EVAL!: "<<E<<endl;
    salir();
  }

  if((MAX_TIME!=0) && (T>MAX_TIME)) //Cuando MAX_TIME==0, no se considera tiempo limite
  {
    cout<<"MAX_TIME!: "<<T<<endl;
    salir();
  }
  return;
}

void calcular_aptitud_calibracion(calibracion *cal_temp){
  for(int i=0; i<NumSeeds; i++){
    int is=int_rand(0,lista_semillas_instancias.size());
    int sem=lista_semillas_instancias[is].seed;
    const char *ins = lista_semillas_instancias[is].instance.c_str();
    calcular_aptitud_semilla_instancia(cal_temp, ins, sem, i);
  }
  return;
}

void calcular_aptitud_conjunto_inicial(conjunto *co){
  for (vector < calibracion >::iterator ca=co->cjto.begin (); ca != co->cjto.end (); ++ca){
    calcular_aptitud_calibracion(&*ca);
  }
  return;
}

int seleccionar_valor_parametro(conjunto * co, int p, int N){
  int pos = int_rand(0, N); //entre 0 y N-1
  int valor=co->cjto[pos].parametro[p];
  return valor;
}

int seleccionar_valor_parametro_por_ruleta(conjunto * co, float * aptitud_temporal, float total, int p){
  float rand=drand48();
  float aptitud_acumulada=0.00;
  int M=co->cjto.size();
    
  for (int i=0; i<M; i++)
  {
   aptitud_acumulada += (aptitud_temporal[i]/total);
   if(rand < aptitud_acumulada)
   {
     int sel=co->cjto[i].parametro[p];
     return sel;
   }
  }
  return (co->cjto.end())->parametro[p];
}

float calcular_aptitud_temporal(conjunto * co, float * aptitud_temporal){
  float total=0.00;
  int maximo = co->cjto.back().aptitud_promedio;
  int minimo = co->cjto.front().aptitud_promedio;
  int i=0;
  for (vector<calibracion>::iterator ca = co->cjto.begin (); ca != co->cjto.end (); ++ca){
    if(minimizar==1)
        aptitud_temporal[i] = minimo + maximo - (ca->aptitud_promedio);
    else
        aptitud_temporal[i] = ca->aptitud_promedio;
    total += aptitud_temporal[i];
    i++;
  } 
  return total;
}

bool mejor(calibracion a, calibracion b){
  if(minimizar==1)
      if((a.aptitud_promedio) < (b.aptitud_promedio)) return true;
      else return false;
  else
      if((a.aptitud_promedio) > (b.aptitud_promedio)) return true;
      else return false;
}

void agregar_calibracion_cruzada_y_calibracion_mutada(conjunto * co){
  int sel;
  int M=co->cjto.size(); //Conservar el mismo tamaño al final del proceso de transformacion
  poblacion_intermedia.cjto.clear();
  float aptitud_temporal[M]; //en caso de que se trate de un problema de minimizacion
  float aptitud_acumulada = calcular_aptitud_temporal(co, aptitud_temporal);
  list <int> valores_presentes;
  if(debug) cout<<"Cruzamiento"<<endl;
  //---------------Cruzamiento-----------------
  calibracion calibracion_cruzada;
  for(int p=0; p<cantidad_parametros; p++){
    if(aptitud_acumulada>0)
      sel=seleccionar_valor_parametro_por_ruleta(co, aptitud_temporal, aptitud_acumulada, p);
    else
      sel=seleccionar_valor_parametro(co, p, M);
      calibracion_cruzada.parametro.push_back(sel);
  }
  calcular_aptitud_calibracion(&calibracion_cruzada);
  poblacion_intermedia.cjto.push_back(calibracion_cruzada);
  if(debug){
    cout<<"calibracion_cruzada"<<endl;
    cout<<calibracion_cruzada<<endl;
    cout<<"calidad-->" << calibracion_cruzada.aptitud_promedio<<endl;
  }
    
  //---------------Mutacion-----------------
  calibracion calibracion_mutada;
  int minimo=0, maximo=0;
  for(int p=0; p<cantidad_parametros; p++)
    calibracion_mutada.parametro.push_back(calibracion_cruzada.parametro[p]);
    calibracion_mutada=calibracion_cruzada;
    //Seleccionar parametro a mutar
    int mut=int_rand(0, cantidad_parametros);
    if(debug) cout<<"Mutando el parametro: "<<mut<<endl;
    int vecino=0;
    do{
      calibracion_mutada.parametro[mut] = int_rand(configuracion_parametros[mut].limite_minimo, (configuracion_parametros[mut].limite_maximo + 1));
      calcular_aptitud_calibracion(&calibracion_mutada);
      if(debug){
        cout<<"vecino "<<vecino<<endl;
        cout<<calibracion_mutada<<endl;
      }
      vecino++;
      if(debug) cout<<"Intento "<< vecino-1<<": Comparando calibracion mutada ("<<calibracion_mutada.aptitud_promedio << ") con calibracion_cruzada (" <<calibracion_cruzada.aptitud_promedio<<")"<<endl;
    }while((!mejor(calibracion_mutada, calibracion_cruzada))
    && (vecino < configuracion_parametros[mut].t_dominio));


    if(mejor(calibracion_mutada, calibracion_cruzada))
    {
      poblacion_intermedia.cjto.push_back(calibracion_mutada);
      if(debug){
        cout<<"Agregando calibracion_mutada a la poblacion "<<endl;
        cout<<calibracion_mutada<<endl;
      }
    }
   
  /*****************************************************/
  //copiar los restantes en conjunto_calibraciones_intermedio

  int c=poblacion_intermedia.cjto.size();
  int tam= co->cjto.size();
  vector <calibracion>::iterator ca=co->cjto.end()-1;
  calibracion aux = *ca;

  for(vector <calibracion>::iterator ca=co->cjto.begin(); ca!=co->cjto.end(), c<M; ++ca, c++){
   poblacion_intermedia.cjto.push_back(*ca);
  }
  co->cjto.clear();
  co->cjto=poblacion_intermedia.cjto;
  if(debug2) cout<<endl<<"Transformation done!!"<<endl;
  return;
}

bool existe_archivo(const char *fileName){
    ifstream infile(fileName);
    return infile.good();
}

int leer_archivo_candidatas(conjunto *co, int tamano){
  int candidatas=0;
  ifstream in(archivo_candidatas);
  string s;
  while(getline(in, s)) 
    if (!(s.compare(" ") == 0))
        candidatas++;
    
  cout<<"Hay "<< candidatas << " soluciones candidatas en el archivo." << endl;
  if(candidatas > tamano){
      cout<<"Se usaran sólo las "<< tamano << " primeras." << endl;
      candidatas = tamano;
  }
  in.close();
  
  FILE *archivo;
  int response;
  archivo=fopen(archivo_candidatas, "r");
  
  configuracion c_temp;
  
  for(int j=0; j<candidatas; j++){
    for(int i=0; i< cantidad_parametros; i++){
      if(configuracion_parametros[i].decimales == 1){ // Entero o categorico
          int v;
          response=fscanf(archivo, "%d", &v);
          if((v > configuracion_parametros[i].limite_maximo) || (v < configuracion_parametros[i].limite_minimo)) {
            cout << "Valor: " << v << " fuera del rango de valores especificado para: " << configuracion_parametros[i].nombre << endl;
            salir();
          }
          else
            co->cjto[j].parametro[i] = v;
      }
      else{ //Real
          float v;
          response=fscanf(archivo, "%f", &v);
          if(((int)(v * configuracion_parametros[i].decimales) > configuracion_parametros[i].limite_maximo) || ((int)(v * configuracion_parametros[i].decimales) < configuracion_parametros[i].limite_minimo)) {
            cout << "Valor: " << v << " fuera del rango de valores especificado para: " << configuracion_parametros[i].nombre << endl;
            salir();
          }
          co->cjto[j].parametro[i] = (int)(v * maximo_decimales);
          
      }
    }
  }
  fclose(archivo);
  if(debug) printf("leer_archivo_candidatas done!\n");
  return candidatas;
}

void crear_conjunto_inicial(conjunto *co, int tamano){
    
  for (int c=0; c<tamano; c++){
    calibracion ca;
    for(int p=0; p<cantidad_parametros; p++)
        ca.parametro.push_back(0);
    co->cjto.push_back(ca);
  }
  int candidatas;

  if(existe_archivo(archivo_candidatas)){
        cout << "Leyendo configuraciones candidatas" << endl;
        candidatas=leer_archivo_candidatas(co, tamano);
  }  
  
  int tamanoi=candidatas;
  
  tamano = tamano - tamanoi;
  
  if (tamano > 0){
    list <int> valores_presentes;
  
    for(int p=0; p<cantidad_parametros; p++){
        valores_presentes.clear();
        configuracion_parametros[p].t_dominio = configuracion_parametros[p].limite_maximo - configuracion_parametros[p].limite_minimo + 1;
        if (configuracion_parametros[p].t_dominio > tamano){
            if(debug) cout<<"Parametro: "<<p<<", necesita mas valores que "<<tamano<<endl;
            if(configuracion_parametros[p].decimales > 1){
                float vd=((float)(maximo_decimales))/configuracion_parametros[p].decimales;
                valores_presentes.push_back(configuracion_parametros[p].limite_maximo*vd);
                valores_presentes.push_back(configuracion_parametros[p].limite_minimo*vd);
                int gap = configuracion_parametros[p].t_dominio / tamano;
                int current = configuracion_parametros[p].limite_minimo;
                for(int c=2; c<tamano;c++){
                    current=current+gap;
                    valores_presentes.push_back(current*vd);
                }
            }
            else{
                valores_presentes.push_back(configuracion_parametros[p].limite_maximo);
                valores_presentes.push_back(configuracion_parametros[p].limite_minimo);
                int gap = configuracion_parametros[p].t_dominio / tamano;
                int current = configuracion_parametros[p].limite_minimo;
                for(int c=2; c<tamano; c++){
                    current=current+gap;
                    valores_presentes.push_back(current);
                }
            }
            configuracion_parametros[p].t_dominio=tamano;
        }
        else{
            if(debug) cout<<"Parametro: "<<p<<", total_valores: "<<configuracion_parametros[p].t_dominio<<endl;
            for(int c=0; c<tamano; c++){
                //if(debug) cout<<"Calibracion "<<c<<endl;
                if(configuracion_parametros[p].decimales > 1){
                    int vi=(configuracion_parametros[p].limite_minimo + c%configuracion_parametros[p].t_dominio);
                    float vd=((float)(maximo_decimales))/configuracion_parametros[p].decimales;
                    int valor=(int)(vi*vd);
                    //if(debug) cout<<"Vi: "<< vi << ", Vd: "<<vd<<" Vf: "<<valor<<endl;
                    valores_presentes.push_back(valor);
                }
                else{
                    int valor=(configuracion_parametros[p].limite_minimo + c%configuracion_parametros[p].t_dominio);
                    //if(debug) cout<<"Valor: "<<valor<<endl;
                    valores_presentes.push_back(valor);
                }
            }
        }
        if(debug){
            for(list <int>::iterator i=valores_presentes.begin(); i!=valores_presentes.end(); ++i)
                cout<< *i << " ";
            cout<<endl;
        }
        for(int c=0; c<tamano; c++){
            int sel = int_rand(0, valores_presentes.size());
            list <int>::iterator pos;
            pos=valores_presentes.begin();
            advance(pos, sel);
            int entero = *pos;
            co->cjto[c+tamanoi].parametro[p] = entero;
            valores_presentes.erase(pos);
        }
    }
  }
  for(int p=0; p<cantidad_parametros; p++){
    if(configuracion_parametros[p].decimales > 1){
      float vd=((float)(maximo_decimales))/configuracion_parametros[p].decimales;
      configuracion_parametros[p].decimales = maximo_decimales;
      configuracion_parametros[p].limite_minimo = (int)(configuracion_parametros[p].limite_minimo*vd);
      configuracion_parametros[p].limite_maximo = (int)(configuracion_parametros[p].limite_maximo*vd);
    }
  }
  if(debug) cout<<*co;
  calcular_aptitud_conjunto_inicial(co);
  if(debug) cout<<*co;
    
  if(debug) printf("crear_conjunto_inicial done!\n");
  return;
}

void transformar(conjunto *co, int iteracion){
  sort(co->cjto.begin(), co->cjto.end(), mejor);
  fstream log;
  log.open(archivo_log, ios::app);
  log << iteracion << " " << co->cjto.at(0);
  log.close();

  if(debug) cout<<*co;
  
  agregar_calibracion_cruzada_y_calibracion_mutada(co);
  
  if(debug) cout<<*co;
  return;
}

int calcular_M(void){
  int Maximo = 0;
  for(int i=0; i<cantidad_parametros; i++){
    if((configuracion_parametros[i].limite_maximo - configuracion_parametros[i].limite_minimo) > Maximo)
      Maximo = configuracion_parametros[i].limite_maximo - configuracion_parametros[i].limite_minimo;
  }
  if (Maximo > MaxM)
    Maximo = MaxM;
  if(Maximo < 10)
    Maximo = 10;
  printf("calcular_M done!\n");
  return Maximo;
}

int contar_instancias_training(char * archivo){
  int lineas=0;
  string s;
  ifstream infile;
  infile.open(archivo); // open file
  while(infile){
    getline(infile,s);
    if(s.length()==0) return lineas;
    else lineas++;
  }
  return lineas;
}
  
int main (int argc, char *argv[])
{
  ejecutable=argv[1];
  archivo_configuracion=argv[2];
  archivo_instancias=argv[3];
  CIT=contar_instancias_training(archivo_instancias);
  if(debug2) cout<<"Contador Instancias Training: "<<CIT<<endl;
  SEED=atoi(argv[4]);
  NumSeeds=atoi(argv[5]);
  MaxM=atoi(argv[6]);

  MAX_EVAL=atoi(argv[7]);
  MAX_TIME=atoi(argv[8]);
  minimizar=atoi(argv[9]);
  archivo_candidatas=argv[10];

  archivo_log="EVOCA.log";
  FILE *log = fopen(archivo_log, "w");
  fclose(log);

  cout<<"------------------------------------------------------------------------"<<endl;
  cout<<"---------------------------- INICIALIZACION ----------------------------"<<endl;
  cout<<"------------------------------------------------------------------------"<<endl;
  
  leer_archivo_configuracion();
  
  cout<<"------------------------------------------------------------------------"<<endl;
  cout<<"-------------------- CREACION CONJUNTO INICIAL -------------------------"<<endl;
  cout<<"------------------------------------------------------------------------"<<endl;
  int M=calcular_M();
  poblacion.id=0;
  
  srand48(SEED);
  int Max_IS;
  if(MAX_EVAL!=0) Max_IS=MAX_EVAL;
  if(MAX_TIME!=0) Max_IS=MAX_TIME*10;
  MAX_ITER=Max_IS;

  for(int i=0; i<Max_IS; i++) //Suficientes para toda la vida
  {
    if(debug2) cout<<"Agregando semilla/instancia: "<<i<<endl;
    agregar_semilla_instancia(&lista_semillas_instancias);
  }

  crear_conjunto_inicial(&poblacion, M);

  if(debug) cout<<poblacion;
  
  for(I = 0; I < MAX_ITER; I++){
    cout<<"------------------------------------------------------------------------"<<endl;
    cout<<"----------------- ITERACION : "<< I <<" -------------------------------------"<<endl;
    cout<<"------------------------------------------------------------------------"<<endl;
    cout<<"-----------------AGREGANDO NUEVAS CALiBRACIONES ------------------------"<<endl;
    cout<<"------------------------------------------------------------------------"<<endl;
    transformar(&poblacion, I);
  }
  cout<<"Max Iteraciones!"<<I<<endl;

  salir();
  return 0;
}
