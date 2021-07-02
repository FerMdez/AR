#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>
using namespace std;

/*
	AR - Ejercicio de análisis RTO
	Fernando Méndez Torrubiano (https://fernmend.ddns.net | @HumperCobra)
*/

//Constantes:
const int nDatos = 200;
const int nSeries = 5;

//Tipos:
typedef struct {
	double 
		M, //Muestra.
		RTTs, //RTTs.
		D, //Desviación.
		RTO_Jacobson, //RTO de Jacobson.
		RTO_Jacobson_Karels; //RTO de Jacobson/Karels.
} tDatos;
typedef tDatos tListaDatos[nDatos];

struct Comma final : std::numpunct<char>{ //Cambiar los '.' de los double por ','.
	char do_decimal_point() const override { return ','; }
};

//Funciones:
void inicializa(tListaDatos&); //Inicializa el array de datos.
bool cargar(tListaDatos&, string&); //Carga los archivos de las series.
void calcular(tListaDatos&, string&); //Calcula los resultados de los algoritmos de Jacobson y Jacobson/Karels.
void guardar(tListaDatos&, string&); //Guarda los resultados de los algoritmos de Jacobson y Jacobson/Karels.



int main() {
	system("chcp 1252"); //Castellano.
	system("cls");

	tListaDatos _datos;
	string serie = "../Series/serie", salida = "../Resultados/output"; //Nombres de los archivos.


	for (int i = 1; i <= nSeries+1; i++){
		serie += to_string(i) + ".dat";
		if (cargar(_datos, serie)) {
			salida += to_string(i) + ".dat";
			calcular(_datos, salida);
			cout << "Serie " << i << " calculada correctamente." << endl;
		}
		else {
			cout << "Error al cargar uno o m�s archivos de datos." << endl;
		}
		serie = "../Series/serie";
		salida = "../Resultados/output";
	}

	//system("PAUSE");
	return 0;
}

void inicializa(tListaDatos& _datos) {
	for (int i = 0; i < nDatos; i++){
		_datos[i].M = 0;
		_datos[i].D = 0;
		_datos[i].RTTs = 0;
		_datos[i].RTO_Jacobson = 0;
		_datos[i].RTO_Jacobson_Karels = 0;
	}
}

bool cargar(tListaDatos& _datos, string& serie) {
	bool carga = false;
	ifstream archivo;

	inicializa(_datos);
	archivo.open(serie);
	if (!archivo.is_open()) {
		cout << "Error al cargar el archivo: " << serie << endl;
		carga = false;
	}
	else {
		int i = 0;
		while (!archivo.eof() && i < nDatos) {
			archivo >> _datos[i].M;
			i++;
		}
		carga = true;
	}
	archivo.close();

	return carga;
}

void calcular(tListaDatos& _datos, string& fichero) {
	double alpha = 0.125; // 1/8
	double beta = 0.25; // 1/4
	double gamma = 2;

	_datos[0].RTTs = _datos[0].M; //Calculamos RTTs de 0.
	_datos[0].D = _datos[0].M / 2; //Calculamos la desviación de 0.
	_datos[0].RTO_Jacobson = gamma * _datos[0].RTTs; //Calculamos RTO de Jacobson de 0.
	_datos[0].RTO_Jacobson_Karels = _datos[0].RTTs + (4 * _datos[0].D); //Calculamos RTO de Jacobson/Karels de 0.
	for (int i = 1; i < nDatos; i++) {
		_datos[i].RTTs = (((1 - alpha) * _datos[i - 1].RTTs) + (alpha * _datos[i].M)); //Calculamos RTTs de i.
		_datos[i].RTO_Jacobson = gamma * _datos[i].RTTs; //Calculamos RTO de Jacobson de i.
		_datos[i].D = (((1 - beta) * _datos[i - 1].D) + (beta * abs(_datos[i - 1].RTTs - _datos[i].M))); //Calculamos la Desviación de i.
		_datos[i].RTO_Jacobson_Karels = (_datos[i].RTTs + (4 * _datos[i].D)); //Calculamos RTO de Jacobson/Karels de i.
	}

	guardar(_datos, fichero);
}


void guardar(tListaDatos& _datos, string& fichero) {
	ofstream archivo;

	archivo.open(fichero);
	if (!archivo.is_open()) {
		cout << "Error al guardar el fichero." << fichero << endl;
	}
	else {
		archivo.imbue(std::locale(std::locale::classic(), new Comma));
		archivo << left 
			<< setw(20) << "RTO(Jacobson)" 
			<< setw(25) << "RTO(Jacobson/Karels)" 
			<< setw(15) << "RTTs" 
			<< setw(15) << "Desviación" 
			<< setw(0) << " M" << endl;
		for (int i = 0; i < nDatos; i++) {
			archivo << left << setprecision(5)
				<< setw(20) << _datos[i].RTO_Jacobson
				<< setw(25) << _datos[i].RTO_Jacobson_Karels 
				<< setw(15) << _datos[i].RTTs 
				<< setw(15) << _datos[i].D
				<< setw(0) << _datos[i].M << endl;
		}
	}
	archivo.close();
}