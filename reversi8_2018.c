#include "stdint.h"

// Tamaño del tablero
enum { DIM=8 };

// Valores que puede devolver la función patron_volteo())
enum {
	NO_HAY_PATRON = 0,
  PATRON_ENCONTRADO = 1
};

// Estados de las casillas del tablero
enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2
};

// candidatas: indica las posiciones a explorar
// Se usa para no explorar tod el tablero innecesariamente
// Sus posibles valores son NO, SI, CASILLA_OCUPADA
const uint8_t  NO              = 0;
const uint8_t  SI              = 1;
const uint8_t  CASILLA_OCUPADA = 2;

enum { NUMPOS=17 };
const uint8_t posicionesF[NUMPOS] = {4,5,6,2,3,3,3,5,1,3,3,0,2,7,7,0,2};
const uint8_t posicionesC[NUMPOS] = {4,1,0,3,2,5,1,3,3,6,0,0,6,0,3,3,6};
int iteration=0;

/////////////////////////////////////////////////////////////////////////////
// TABLAS AUXILIARES
// declaramos las siguientes tablas como globales para que sean más fáciles visualizarlas en el simulador
// __attribute__ ((aligned (8))): specifies a minimum alignment for the variable or structure field, measured in bytes, in this case 8 bytes

static const uint8_t __attribute__ ((aligned (8))) tabla_valor[DIM][DIM] =
{
    {8,2,7,3,3,7,2,8},
    {2,1,4,4,4,4,1,2},
    {7,4,6,5,5,6,4,7},
    {3,4,5,0,0,5,4,3},
    {3,4,5,0,0,5,4,3},
    {7,4,6,5,5,6,4,7},
    {2,1,4,4,4,4,1,2},
    {8,2,7,3,3,7,2,8}
};


// Tabla de direcciones. Contiene los desplazamientos de las 8 direcciones posibles
const uint8_t vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
const uint8_t vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};

//////////////////////////////////////////////////////////////////////////////////////
// Variables globales que no deberían serlo
// tablero, fila, columna y ready son varibles que se deberían definir como locales dentro de reversi8.
// Sin embargo, las hemos definido como globales para que sea más fácil visualizar el tablero y las variables en la memoria
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Tablero sin inicializar
////////////////////////////////////////////////////////////////////
uint8_t __attribute__ ((aligned (8))) tablero[DIM][DIM] = {
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA}
	    };

  ////////////////////////////////////////////////////////////////////
     // VARIABLES PARA INTERACCIONAR CON LA ENTRADA SALIDA
     // Pregunta: ¿hay que hacer algo con ellas para que esto funcione bien?
     // (por ejemplo añadir alguna palabra clave para garantizar que la sincronización a través de esa variable funcione)
  uint8_t fila=0, columna=0, ready = 0;



extern int patron_volteo_arm_c(uint8_t tablero[][8], int *longitud,uint8_t f, uint8_t c, uint8_t SF, uint8_t SC, uint8_t color);

extern int patron_volteo_arm_arm(uint8_t tablero[][8], int *longitud,uint8_t f, uint8_t c, uint8_t SF, uint8_t SC, uint8_t color);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0 indica CASILLA_VACIA, 1 indica FICHA_BLANCA y 2 indica FICHA_NEGRA
// pone el tablero a cero y luego coloca las fichas centrales.
void init_table(uint8_t tablero[][DIM], uint8_t candidatas[][DIM])
{
    int i, j;

    for (i = 0; i < DIM; i++)
    {
        for (j = 0; j < DIM; j++)
            tablero[i][j] = CASILLA_VACIA;  candidatas[i][j] = NO;
    }
#if 0
    for (i = 3; i < 5; ++i) {
	for(j = 3; j < 5; ++j) {
	    tablero[i][j] = i == j ? FICHA_BLANCA : FICHA_NEGRA;
	}
    }

    for (i = 2; i < 6; ++i) {
	for (j = 2; j < 6; ++j) {
	    if((i>=3) && (i < 5) && (j>=3) && (j<5)) {
		candidatas[i][j] = CASILLA_OCUPADA;
	    } else {
		candidatas[i][j] = SI; //CASILLA_LIBRE;
	    }
	}
    }
#endif
    // arriba hay versión alternativa
    tablero[3][3] = FICHA_BLANCA;
    tablero[4][4] = FICHA_BLANCA;
    tablero[3][4] = FICHA_NEGRA;
    tablero[4][3] = FICHA_NEGRA;

    candidatas[3][3] = CASILLA_OCUPADA;
    candidatas[4][4] = CASILLA_OCUPADA;
    candidatas[3][4] = CASILLA_OCUPADA;
    candidatas[4][3] = CASILLA_OCUPADA;

    // casillas a explorar:
    candidatas[2][2] = SI;
    candidatas[2][3] = SI;
    candidatas[2][4] = SI;
    candidatas[2][5] = SI;
    candidatas[3][2] = SI;
    candidatas[3][5] = SI;
    candidatas[4][2] = SI;
    candidatas[4][5] = SI;
    candidatas[5][2] = SI;
    candidatas[5][3] = SI;
    candidatas[5][4] = SI;
    candidatas[5][5] = SI;
}

void esperar_mov(uint8_t *ready)
{
    while (*ready == 0) {};  // bucle de espera de respuestas hasta que el se modifique el valor de ready (hay que hacerlo manualmente)

    *ready = 0;  //una vez que pasemos el bucle volvemos a fijar ready a 0;
}

////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// IMPORTANTE: AL SUSTITUIR FICHA_VALIDA() Y PATRON_VOLTEO()
// POR RUTINAS EN ENSAMBLADOR HAY QUE RESPETAR LA MODULARIDAD.
// DEBEN SEGUIR SIENDO LLAMADAS A FUNCIONES Y DEBEN CUMPLIR CON EL ATPCS
// (VER TRANSPARENCIAS Y MATERIAL DE PRACTICAS):
//  - DEBEN PASAR LOS PARAMETROS POR LOS REGISTROS CORRESPONDIENTES
//  - GUARDAR EN PILA SOLO LOS REGISTROS QUE TOCAN
//  - CREAR UN MARCO DE PILA TAL Y COMO MUESTRAN LAS TRANSPARENCIAS
//    DE LA ASIGNATURA (CON EL PC, FP, LR,....)
//  - EN EL CASO DE LAS VARIABLES LOCALES, SOLO HAY QUE APILARLAS
//    SI NO SE PUEDEN COLOCAR EN UN REGISTRO.
//    SI SE COLOCAN EN UN REGISTRO NO HACE FALTA
//    NI GUARDARLAS EN PILA NI RESERVAR UN ESPACIO EN LA PILA PARA ELLAS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////////////////////////////////////////////////////////////////////////////////
// Devuelve el contenido de la posición indicadas por la fila y columna actual.
// Además informa si la posición es válida y contiene alguna ficha.
// Esto lo hace por referencia (en *posicion_valida)
// Si devuelve un 0 no es válida o está vacia.
uint8_t ficha_valida(uint8_t tablero[][DIM], uint8_t f, uint8_t c, int *posicion_valida)
{
    uint8_t ficha;

    // ficha = tablero[f][c];
    // no puede accederse a tablero[f][c]
    // ya que algún índice puede ser negativo

    if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
    {
        *posicion_valida = 1;
        ficha = tablero[f][c];
    }
    else
    {
        *posicion_valida = 0;
        ficha = CASILLA_VACIA;
    }
    return ficha;
}

////////////////////////////////////////////////////////////////////////////////
// La función patrón volteo es una función recursiva que busca el patrón de volteo
// (n fichas del rival seguidas de una ficha del jugador actual) en una dirección determinada
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// la función devuelve PATRON_ENCONTRADO (1) si encuentra patrón y NO_HAY_PATRON (0) en caso contrario
// FA y CA son la fila y columna a analizar
// longitud es un parámetro por referencia. Sirve para saber la longitud del patrón que se está analizando. Se usa para saber cuantas fichas habría que voltear
int patron_volteo(uint8_t tablero[][DIM], int *longitud, uint8_t FA, uint8_t CA, uint8_t SF, uint8_t SC, uint8_t color)
{
    int posicion_valida; // indica si la posición es valida y contiene una ficha de algún jugador
    int patron; //indica si se ha encontrado un patrón o no
    uint8_t casilla;   // casilla es la casilla que se lee del tablero
    FA = FA + SF;
    CA = CA + SC;
    casilla = ficha_valida(tablero, FA, CA, &posicion_valida);
    // mientras la casilla está en el tablero, no está vacía,
    // y es del color rival seguimos buscando el patron de volteo
    if ((posicion_valida == 1) && (casilla != color))
    {
        *longitud = *longitud + 1;
        patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color);

        return patron;
    }
    // si la ultima posición era válida y la ficha es del jugador actual,
    // entonces hemos encontrado el patrón
    else if ((posicion_valida == 1) && (casilla == color))
    {
        if (*longitud > 0) // longitud indica cuantas fichas hay que voltear
            {
            return PATRON_ENCONTRADO; // si hay que voltear una ficha o más hemos encontrado el patrón
            //printf("PATRON_ENCONTRADO \n");
            }
        else {
            return NO_HAY_PATRON; // si no hay que voltear no hay patrón
            //printf("NO_HAY_PATRON \n");
            }
    }
    // en caso contrario es que no hay patrón
    else
    {
        return NO_HAY_PATRON;
        //printf("NO_HAY_PATRON \n");
    }
}
////////////////////////////////////////////////////////////////////////////////
// voltea n fichas en la dirección que toque
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// FA y CA son la fila y columna a analizar
void voltear(uint8_t tablero[][DIM], uint8_t FA, uint8_t CA, uint8_t SF, uint8_t SC, int n, uint8_t color)
{
    int i;

    for (i = 0; i < n; i++)
    {
        FA = FA + SF;
        CA = CA + SC;
        tablero[FA][CA] = color;
    }
}

int patron_volteo_test(uint8_t tablero[][DIM], int *longitud, uint8_t FA, uint8_t CA, uint8_t SF, uint8_t SC, uint8_t color)
{
	int longitudAux=*longitud;
	int longitudAux2=*longitud;
	int encontrado=patron_volteo(tablero, longitud, FA, CA, SF, SC, color);
	int encontradoAux=patron_volteo_arm_c(tablero, &longitudAux, FA, CA, SF, SC, color);

	if (encontrado!=encontradoAux){
		while(1);	//valor de la funcion devuelto por patron_volteo_arm_c no coincide
	}
	if(*longitud!=longitudAux){
		while(1);	//valor de longitud devuelto por patron_volteo_arm_c no coincide
	}

	encontradoAux=patron_volteo_arm_arm(tablero, &longitudAux2, FA, CA, SF, SC, color);

	if (encontrado!=encontradoAux){
		while(1);	//valor de la funcion devuelto por patron_volteo_arm_arm no coincide
	}
	if(*longitud!=longitudAux2){
		while(1);	//valor de longitud devuelto por patron_volteo_arm_arm no coincide
	}
	return encontrado;
}




////////////////////////////////////////////////////////////////////////////////
// comprueba si hay que actualizar alguna ficha
// no comprueba que el movimiento realizado sea válido
// f y c son la fila y columna a analizar
// uint8_t vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
// uint8_t vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};
int actualizar_tablero(uint8_t tablero[][DIM], uint8_t f, uint8_t c, uint8_t color)
{
    uint8_t SF, SC; // cantidades a sumar para movernos en la dirección que toque
    int i, flip, patron;

    for (i = 0; i < DIM; i++) // 0 es Norte, 1 NE, 2 E ...
    {
        SF = vSF[i];
        SC = vSC[i];
        // flip: numero de fichas a voltear
        flip = 0;
        patron = patron_volteo_test(tablero, &flip, f, c, SF, SC, color);

        if (patron == PATRON_ENCONTRADO )
        {
            voltear(tablero, f, c, SF, SC, flip, color);
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
// Recorre tod el tablero comprobando en cada posición si se puede mover
// En caso afirmativo, consulta la puntuación de la posición y si es la mejor
// que se ha encontrado la guarda
// Al acabar escribe el movimiento seleccionado en f y c

// Candidatas
// NO    0
// SI    1
// CASILLA_OCUPADA 2
int elegir_mov(uint8_t candidatas[][DIM], uint8_t tablero[][DIM], uint8_t *f, uint8_t *c)
{
    int i, j, k, found;
    int mf = -1; // almacena la fila del mejor movimiento encontrado
    int mc;      // almacena la columna del mejor movimiento encontrado
    uint8_t mejor = 0; // almacena el mejor valor encontrado
    int patron, longitud;
    uint8_t SF, SC; // cantidades a sumar para movernos en la dirección que toque

    // Recorremos tod el tablero comprobando dónde podemos mover
    // Comparamos la puntuación de los movimientos encontrados y nos quedamos con el mejor
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {   // indica en qué casillas quizá se pueda mover
            if (candidatas[i][j] == SI)
            {
                if (tablero[i][j] == CASILLA_VACIA)
                {
                    found = 0;
                    k = 0;

                    // en este bucle comprobamos si es un movimiento válido
                    // (es decir si implica voltear en alguna dirección)
                    while ((found == 0) && (k < DIM))
                    {
                        SF = vSF[k];    // k representa la dirección que miramos
                        SC = vSC[k];    // 1 es norte, 2 NE, 3 E ...

                        // nos dice qué hay que voltear en cada dirección
                        longitud = 0;
                        patron = patron_volteo_test(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA);


                        if (patron == PATRON_ENCONTRADO)
                        {
                            found = 1;
                            if (tabla_valor[i][j] > mejor)
                            {
                                mf = i;
                                mc = j;
                                mejor = tabla_valor[i][j];
                            }
                        }
                        k++;
                        // si no hemos encontrado nada probamos con la siguiente dirección
                    }
                }
            }
        }
    }
    *f = (uint8_t) mf;
    *c = (uint8_t) mc;
    // si no se ha encontrado una posición válida devuelve -1
    return mf;
}
////////////////////////////////////////////////////////////////////////////////
// Cuenta el número de fichas de cada color.
// Los guarda en la dirección b (blancas) y n (negras)
void contar(uint8_t tablero[][DIM], int *b, int *n)
{
    int i,j;

    *b = 0;
    *n = 0;

    // recorremos tod el tablero contando las fichas de cada color
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {
            if (tablero[i][j] == FICHA_BLANCA)
            {
                (*b)++;
            }
            else if (tablero[i][j] == FICHA_NEGRA)
            {
                (*n)++;
            }
        }
    }
}

void actualizar_candidatas(uint8_t candidatas[][DIM], uint8_t f, uint8_t c)
{
    // donde ya se ha colocado no se puede volver a colocar
    // En las posiciones alrededor sí
    candidatas[f][c] = CASILLA_OCUPADA;
    if (f > 0)
    {
        if (candidatas[f-1][c] != CASILLA_OCUPADA)
            candidatas[f-1][c] = SI;

        if ((c > 0) && (candidatas[f-1][c-1] != CASILLA_OCUPADA))
            candidatas[f-1][c-1] = SI;

        if ((c < 7) && (candidatas[f-1][c+1] != CASILLA_OCUPADA))
            candidatas[f-1][c+1] = SI;
    }
    if (f < 7)
    {
        if (candidatas[f+1][c] != CASILLA_OCUPADA)
            candidatas[f+1][c] = SI;

        if ((c > 0) && (candidatas[f+1][c-1] != CASILLA_OCUPADA))
            candidatas[f+1][c-1] = SI;

        if ((c < 7) && (candidatas[f+1][c+1] != CASILLA_OCUPADA))
            candidatas[f+1][c+1] = SI;
    }
    if ((c > 0) && (candidatas[f][c-1] != CASILLA_OCUPADA))
        candidatas[f][c-1] = SI;

    if ((c < 7) && (candidatas[f][c+1] != CASILLA_OCUPADA))
        candidatas[f][c+1] = SI;
}






void verificarPartidasAuto(uint8_t tablero[][DIM], uint8_t candidatas[][DIM], int numeroDeMovimientos){
	////////////////////////////////////////////////////////////////////
	 // Tablero candidatas: se usa para no explorar todas las posiciones del tablero
	// sólo se exploran las que están alrededor de las fichas colocadas
	 ////////////////////////////////////////////////////////////////////

	int done;     // la máquina ha conseguido mover o no
	int move = 0; // el humano ha conseguido mover o no
	int blancas, negras; // número de fichas de cada color
	int fin = 0;  // fin vale 1 si el humano no ha podido mover
				  // (ha introducido un valor de movimiento con algún 8)
				  // y luego la máquina tampoco puede
	uint8_t f, c;    // fila y columna elegidas por la máquina para su movimiento

	// PROBAR MOVIMMIENTOS
	while (fin == 0 && numeroDeMovimientos > 0)
	{
		move = 0;
		fila=posicionesF[iteration];
		columna=posicionesC[iteration];
		iteration++;
		ready=1;
		esperar_mov(&ready);
		// si la fila o columna son 8 asumimos que el jugador no puede mover
		if (((fila) != DIM) && ((columna) != DIM))
		{
			tablero[fila][columna] = FICHA_NEGRA;
			actualizar_tablero(tablero, fila, columna, FICHA_NEGRA);
			actualizar_candidatas(candidatas, fila, columna);
			move = 1;
		}

		// escribe el movimiento en las variables globales fila columna
		done = elegir_mov(candidatas, tablero, &f, &c);
		if (done == -1)
		{
			if (move == 0)
				fin = 1;
		}
		else
		{
			tablero[f][c] = FICHA_BLANCA;
			actualizar_tablero(tablero, f, c, FICHA_BLANCA);
			actualizar_candidatas(candidatas, f, c);
		}

		numeroDeMovimientos--;
	}
	contar(tablero, &blancas, &negras);
}






void partidas_test ()
{
	uint8_t __attribute__ ((aligned (8))) candidatas[DIM][DIM] =
		{
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO},
			{NO,NO,NO,NO,NO,NO,NO,NO}
		};



/***********************************************************************************/



	/*
		VOLTEO DE COLUMNAS:

		  0 1 2 3 4 5 6 7
		0{ , , , , , , , },
		1{ , , ,█, , ,▒, },
		2{ , , ,▒, , ,▒, },
		3{ , , ,▒, , ,▒, },
		4{ , , ,▒, , ,▒, },
		5{ ,▒, , , , ,▒, },
		6{ ,█, , , , ,▒, },
		7{ , , , , , ,█, }

	*/

	// CREACIÓN DEL TABLERO DE PRUEBAS:
	int i, j;

	for (i = 0; i < DIM; i++)
	{
		for (j = 0; j < DIM; j++)
			tablero[i][j] = CASILLA_VACIA;
	}

	tablero[6][1] = FICHA_BLANCA;	candidatas[6][1] = CASILLA_OCUPADA;
	tablero[7][1] = FICHA_NEGRA;	candidatas[7][1] = CASILLA_OCUPADA;

	tablero[1][3] = FICHA_NEGRA;	candidatas[1][3] = CASILLA_OCUPADA;
	tablero[2][3] = FICHA_BLANCA;	candidatas[2][3] = CASILLA_OCUPADA;
	tablero[3][3] = FICHA_BLANCA;	candidatas[3][3] = CASILLA_OCUPADA;
	tablero[4][3] = FICHA_BLANCA;	candidatas[4][3] = CASILLA_OCUPADA;

	tablero[1][6] = FICHA_BLANCA;	candidatas[1][6] = CASILLA_OCUPADA;
	tablero[2][6] = FICHA_BLANCA;	candidatas[2][6] = CASILLA_OCUPADA;
	tablero[3][6] = FICHA_BLANCA;	candidatas[3][6] = CASILLA_OCUPADA;
	tablero[4][6] = FICHA_BLANCA;	candidatas[4][6] = CASILLA_OCUPADA;
	tablero[5][6] = FICHA_BLANCA;	candidatas[5][6] = CASILLA_OCUPADA;
	tablero[6][6] = FICHA_BLANCA;	candidatas[6][6] = CASILLA_OCUPADA;
	tablero[7][6] = FICHA_NEGRA;	candidatas[7][6] = CASILLA_OCUPADA;

	// MOVIMIENTOS A COMPROBAR:
	verificarPartidasAuto(tablero, candidatas, 3);


/***********************************************************************************/
	/*
		VOLTEO DE FILAS:
		(IA no puede mover)
		{ , , , , , , , },
		{ , , ,▒,█, , , },
		{ , , , , , , , },
		{ , , , , , , , },
		{ ,█,▒,▒,▒,▒, , },
		{ , , , , , , , },
		{ ,▒,▒,▒,▒,▒,▒,█},
		{ , , , , , , , }

	*/
	// CREACIÓN DEL TABLERO DE PRUEBAS:

	for (i = 0; i < DIM; i++)
	{
		for (j = 0; j < DIM; j++)
			tablero[i][j] = CASILLA_VACIA; candidatas[i][j] = NO;
	}

	tablero[6][1] = FICHA_BLANCA;	candidatas[1][3] = CASILLA_OCUPADA;
	tablero[7][1] = FICHA_NEGRA;	candidatas[1][4] = CASILLA_OCUPADA;

	tablero[7][1] = FICHA_NEGRA;	candidatas[5][4] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[5][5] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[5][6] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[5][7] = CASILLA_OCUPADA;

	tablero[6][1] = FICHA_BLANCA;	candidatas[7][1] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[7][2] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[7][3] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[7][4] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[7][5] = CASILLA_OCUPADA;
	tablero[6][1] = FICHA_BLANCA;	candidatas[7][6] = CASILLA_OCUPADA;
	tablero[7][1] = FICHA_NEGRA;	candidatas[7][7] = CASILLA_OCUPADA;

	// MOVIMIENTOS A COMPROBAR:





/***********************************************************************************/
	/*
		VOLTEO DIAGONALES:
		(IA no puede mover)
			{ , , , , , , ,█},
			{ ,▒, , , , ,▒, },
			{ , ,▒, , ,▒, , },
			{ ,▒, ,▒,▒, , , },
			{ , , ,▒,▒, ,▒, },
			{ , ,▒, , ,▒, , },
			{ ,▒, , , , ,▒, },
			{ , , , , , , ,█}

	*/
	// CREACIÓN DEL TABLERO DE PRUEBAS:


	for (i = 0; i < DIM; i++)
	{
		for (j = 0; j < DIM; j++)
			tablero[i][j] = CASILLA_VACIA; candidatas[i][j] = NO;
	}

	tablero[1][1] = FICHA_BLANCA;	candidatas[1][1] = CASILLA_OCUPADA;
	tablero[2][2] = FICHA_BLANCA;	candidatas[2][2] = CASILLA_OCUPADA;
	tablero[3][3] = FICHA_BLANCA;	candidatas[3][3] = CASILLA_OCUPADA;
	tablero[4][4] = FICHA_BLANCA;	candidatas[4][4] = CASILLA_OCUPADA;
	tablero[5][5] = FICHA_BLANCA;	candidatas[5][5] = CASILLA_OCUPADA;
	tablero[6][6] = FICHA_BLANCA;	candidatas[6][6] = CASILLA_OCUPADA;
	tablero[7][7] = FICHA_NEGRA;	candidatas[7][7] = CASILLA_OCUPADA;

	tablero[6][1] = FICHA_BLANCA;	candidatas[6][1] = CASILLA_OCUPADA;
	tablero[5][2] = FICHA_BLANCA;	candidatas[5][2] = CASILLA_OCUPADA;
	tablero[4][3] = FICHA_BLANCA;	candidatas[4][3] = CASILLA_OCUPADA;
	tablero[3][4] = FICHA_BLANCA;	candidatas[3][4] = CASILLA_OCUPADA;
	tablero[2][5] = FICHA_BLANCA;	candidatas[2][5] = CASILLA_OCUPADA;
	tablero[1][6] = FICHA_BLANCA;	candidatas[1][6] = CASILLA_OCUPADA;
	tablero[0][7] = FICHA_NEGRA;	candidatas[0][7] = CASILLA_OCUPADA;

	tablero[3][1] = FICHA_BLANCA;	candidatas[3][1] = CASILLA_OCUPADA;

	tablero[4][6] = FICHA_BLANCA;	candidatas[4][6] = CASILLA_OCUPADA;


	// MOVIMIENTOS A COMPROBAR:




/***********************************************************************************/
	/*
		VOLTEO VARIOS:
			{ ,▒,▒,▒,▒,▒,▒,█},
			{▒,▒, , , , , , },
			{▒, ,▒, , , , , },
			{▒, , ,▒, , , , },
			{▒, , , ,▒, , , },
			{▒, , , , ,▒, , },
			{▒, , , , , ,▒, },
			{█, ,█,█,▒, , ,█}

	*/
	// CREACIÓN DEL TABLERO DE PRUEBAS:

	for (i = 0; i < DIM; i++)
		{
			for (j = 0; j < DIM; j++)
				tablero[i][j] = CASILLA_VACIA; candidatas[i][j] = NO;
		}

	tablero[1][1] = FICHA_BLANCA;	candidatas[1][1] = CASILLA_OCUPADA;
	tablero[2][2] = FICHA_BLANCA;	candidatas[2][2] = CASILLA_OCUPADA;
	tablero[3][3] = FICHA_BLANCA;	candidatas[3][3] = CASILLA_OCUPADA;
	tablero[4][4] = FICHA_BLANCA;	candidatas[4][4] = CASILLA_OCUPADA;
	tablero[5][5] = FICHA_BLANCA;	candidatas[5][5] = CASILLA_OCUPADA;
	tablero[6][6] = FICHA_BLANCA;	candidatas[6][6] = CASILLA_OCUPADA;
	tablero[7][7] = FICHA_NEGRA;	candidatas[7][7] = CASILLA_OCUPADA;

	tablero[0][1] = FICHA_BLANCA;	candidatas[0][1] = CASILLA_OCUPADA;
	tablero[0][2] = FICHA_BLANCA;	candidatas[0][2] = CASILLA_OCUPADA;
	tablero[0][3] = FICHA_BLANCA;	candidatas[0][3] = CASILLA_OCUPADA;
	tablero[0][4] = FICHA_BLANCA;	candidatas[0][4] = CASILLA_OCUPADA;
	tablero[0][5] = FICHA_BLANCA;	candidatas[0][5] = CASILLA_OCUPADA;
	tablero[0][6] = FICHA_BLANCA;	candidatas[0][6] = CASILLA_OCUPADA;
	tablero[0][7] = FICHA_NEGRA;	candidatas[0][7] = CASILLA_OCUPADA;

	tablero[1][0] = FICHA_BLANCA;	candidatas[1][0] = CASILLA_OCUPADA;
	tablero[2][0] = FICHA_BLANCA;	candidatas[2][0] = CASILLA_OCUPADA;
	tablero[3][0] = FICHA_BLANCA;	candidatas[3][0] = CASILLA_OCUPADA;
	tablero[4][0] = FICHA_BLANCA;	candidatas[4][0] = CASILLA_OCUPADA;
	tablero[5][0] = FICHA_BLANCA;	candidatas[5][0] = CASILLA_OCUPADA;
	tablero[6][0] = FICHA_BLANCA;	candidatas[6][0] = CASILLA_OCUPADA;
	tablero[7][0] = FICHA_NEGRA;	candidatas[7][0] = CASILLA_OCUPADA;


	tablero[7][4] = FICHA_BLANCA;	candidatas[6][6] = CASILLA_OCUPADA;
	tablero[7][2] = FICHA_NEGRA;	candidatas[7][7] = CASILLA_OCUPADA;
	tablero[7][3] = FICHA_NEGRA;	candidatas[7][7] = CASILLA_OCUPADA;

	// MOVIMIENTOS A COMPROBAR:



	/*
	VOLTEO TODOS:
		{ ,█, , , ,█, , },
		{ , ,▒, , ,▒, , },
		{ , , ,▒, ,▒, ,█},
		{ , , , ,▒,▒,▒, },
		{█,▒,▒,▒,▒, ,▒,█},
		{ , , , ,▒,▒,▒, },
		{ , , ,▒, ,▒, ,█},
		{ , ,█, , ,█, , }

	*/

//---------------------------------------------------------------------------------------------------------------------------//
/*
	PARTIDA NORMAL (1):
		{ , , , , , , , },
		{ , , , , , , , },
		{ , , , , , , , },
		{ , , ,▒,█, , , },
		{ , , ,█,▒, , , },
		{ , , , , , , , },
		{ , , , , , , , },
		{ , , , , , , , }
*/

	init_table(tablero, candidatas);

// MOVIMIENTOS A COMPROBAR:
	verificarPartidasAuto(tablero, candidatas, 14);



}


////////////////////////////////////////////////////////////////////////////////
// Proceso principal del juego
// Utiliza el tablero,
// y las direcciones en las que indica el jugador la fila y la columna
// y la señal de ready que indica que se han actualizado fila y columna
// tablero, fila, columna y ready son variables globales aunque deberían ser locales de reversi8,
// la razón es que al meterlas en la pila no las pone juntas, y así jugar es más complicado.
// en esta versión el humano lleva negras y la máquina blancas
// no se comprueba que el humano mueva correctamente.
// Sólo que la máquina realice un movimiento correcto.
void reversi8()
{
	partidas_test();
}
