#ifndef externo
int cantidad_parametros;
char * archivo_log;
#endif

#ifdef externo
extern int cantidad_parametros;
extern char * archivo_log;
#endif

#include <vector>
#include <ostream>

struct si
{
  int seed;
  string instance;
};

struct configuracion
{
  char nombre[30];
  int limite_minimo, limite_maximo;
  int decimales;
  int t_dominio;
};

class calibracion
{
  public:
  float aptitud_promedio;
  std::vector<int> parametro;
  calibracion();
  ~calibracion(){};
  calibracion& operator=(const calibracion &rhs);
  int operator==(const calibracion &rhs) const;
  bool operator<(const calibracion &rhs) const;
  friend std::ostream& operator<<(std::ostream &, const calibracion &);
};

class conjunto
{ 
  public: 
  int id;
  std::vector <calibracion> cjto;
  void vaciar(void);
  void ordenar(void);
  friend std::ostream& operator<<(std::ostream &, const conjunto &);
};

class poblacion
{
  public:
  int id;
  std::vector <conjunto> cjt;
  void vaciar(void);
  friend std::ostream& operator<<(std::ostream &, const poblacion &);
};

/*******************************************************************/

#ifndef externo
vector <si> lista_semillas_instancias;
int I, E=0, T=0;
poblacion Poblacion, Poblacion_temporal;
char *archivo_configuracion;
char *archivo_instancias;
char *archivo_candidatas;
char *ejecutable;
struct configuracion *configuracion_parametros;
int **valores_parametros;
conjunto poblacion, poblacion_intermedia;
int NumSeeds;
int SEED;
int MaxM;
int maximo_decimales;
int MAX_ITER, MAX_EVAL, MAX_TIME;
int CIT;
int minimizar;
#endif

void generar_y_ordenar_matriz_valores_parametros(int, int **);

/*******************************************************************/

