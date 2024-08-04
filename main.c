#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sudoku[9][9];

void leerSudoku(char *fileName, int sudoku[9][9]) {
  FILE *fp;
  int i, j;
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("Error al abrir el archivo\n");
    exit(1);
  }
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      fscanf(fp, "%d", &sudoku[i][j]);
    }
  }
  fclose(fp);
}

void imprimirSudoku(int sudoku[9][9]);

typedef struct {
  int (*sudoku)[9][9];
  // 0 si no es correcto, 1 si es correcto
  // Por defecto es correcto, pero se cambia a 0 cuando se detecta un error
  int correcto;
} DatosHilo;

void validarColumnas(void *arg) {
  printf("Validando columnas\n");

  DatosHilo *data = (DatosHilo *)arg;
  int columnasValidas[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  for (int i = 0; i < 9; i++) {
    int suma = 0;
    for (int j = 0; j < 9; j++) {
      int valor = (*data->sudoku)[j][i];
      suma += valor;
    }
    if (suma != 45) {
      columnasValidas[i] = 0;
      printf("Error en la columna %d\n", i + 1);
    }
  }

  for (int i = 0; i < 9; i++) {
    if (columnasValidas[i] == 0) {
      data->correcto = 0;
      return;
    }
  }
}

void validarFilas(void *arg) {
  printf("Validando filas\n");

  DatosHilo *data = (DatosHilo *)arg;
  int filasValidas[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  for (int i = 0; i < 9; i++) {
    int suma = 0;
    for (int j = 0; j < 9; j++) {
      int valor = (*data->sudoku)[i][j];
      suma += valor;
    }
    if (suma != 45) {
      filasValidas[i] = 0;
      printf("Error en la fila %d\n", i + 1);
    }
  }

  for (int i = 0; i < 9; i++) {
    if (filasValidas[i] == 0) {
      data->correcto = 0;
      return;
    }
  }
}

void validarSubCuadros(void *arg) {
  printf("Validando 3x3\n");
  DatosHilo *data = (DatosHilo *)arg;

  // int subCuadrosValidos[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  for (int i = 0; i < 9; i += 3) {
    for (int j = 0; j < 9; j += 3) {
      int suma = 0;
      // Una vez en el numero superior izquierdo de cada sub-cuadro
      // se suman los valores de cada celda
      // 1 2 3
      // 4 5 6
      // 7 8 9
      suma += (*data->sudoku)[i][j];         // 1
      suma += (*data->sudoku)[i][j + 1];     // 2
      suma += (*data->sudoku)[i][j + 2];     // 3
      suma += (*data->sudoku)[i + 1][j];     // 4
      suma += (*data->sudoku)[i + 1][j + 1]; // 5
      suma += (*data->sudoku)[i + 1][j + 2]; // 6
      suma += (*data->sudoku)[i + 2][j];     // 7
      suma += (*data->sudoku)[i + 2][j + 1]; // 8
      suma += (*data->sudoku)[i + 2][j + 2]; // 9

      if (suma != 45) {
        data->correcto = 0;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Debes especificar el nombre del archivo\n");
    exit(1);
  }

  char *fileName = argv[1];
  leerSudoku(fileName, sudoku);
  imprimirSudoku(sudoku);
  DatosHilo datosColumnas = {&sudoku, 1};
  DatosHilo datosFilas = {&sudoku, 1};
  DatosHilo datosSubCuadros = {&sudoku, 1};
  pthread_t threadColumnas, threadFilas, threadSubCuadros;
  pthread_create(&threadColumnas, NULL, (void *)&validarColumnas,
                 &datosColumnas);
  pthread_create(&threadFilas, NULL, (void *)&validarFilas, &datosFilas);
  pthread_create(&threadSubCuadros, NULL, (void *)&validarSubCuadros,
                 &datosSubCuadros);
  pthread_join(threadColumnas, NULL);
  pthread_join(threadFilas, NULL);
  pthread_join(threadSubCuadros, NULL);

  if (datosColumnas.correcto && datosFilas.correcto &&
      datosSubCuadros.correcto) {
    printf("Correcto\n");
  } else {
    printf("Incorrecto\n");
  }

  return 0;
}

// UTILIDADES
void separadorFilas(int n) {
  printf(" ");
  for (int i = 0; i < n + 2; i++) {
    printf("--");
  }
  printf("-\n");
}

void imprimirSudoku(int sudoku[9][9]) {

  for (int i = 0; i < 9; i++) {
    if (i % 3 == 0) {
      separadorFilas(9);
    }

    for (int j = 0; j < 9; j++) {
      if (j % 3 == 0) {
        printf("| ");
      }
      printf("%d ", sudoku[i][j]);
    }
    printf("|\n");
  }

  separadorFilas(9);
}
