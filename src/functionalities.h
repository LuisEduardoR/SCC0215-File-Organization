// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

#ifndef FUNCTIONALITIES_H
# define FUNCTIONALITIES_H

// Executa a funcionalidade 1 do Trabalho 1 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo .csv e os escreve em um binário de acordo com a especificação.
int functionality1_CsvToBin(FILE *file);

// Executa a funcionalidade 2 do Trabalho 1 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo binário e exibe seus registros na tela.
int functionality2_printBin(FILE *file);

// Executa a funcionalidade 3 do Trabalho 1 da disciplina SCC0215 - Organizacao de Arquivos.
// Busca registros com um determinado valor em um determinado campo em um arquivo binário e os exibe na tela.
int functionality3_printBinSelect(FILE *file);

// Executa a funcionalidade 4 e 12 dos Trabalhos 2 e 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Remove registros com um determinado valor em um determinado campo de uma rquivo binário e atualiza a lista encadeada de registros removidos
// para que seu espaço possa ser reaproveitado, também atualzia o índice se useINdex != 0.
int functionality4_12_removeRegisters(FILE *file, int useIndex);

// Executa a funcionalidade 5 do Trabalho 2 da disciplina SCC0215 - Organizacao de Arquivos.
// Insere registros em um arquivo binário fazendo uso do reaproveitamento de registros removidos quando possível.
int functionality5_13_insertRegisters(FILE *file, int useIndex);

// Executa a funcionalidade 6 do Trabalho 2 da disciplina SCC0215 - Organizacao de Arquivos.
// Atualiza registros do arquivo binário, os removendo e reinserindo quando necessário.
int functionality6_updateRegisters(FILE *file);

// Executa a funcionalidade 7 do Trabalho 3 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo binário, ordena seus registros e salva um novo arquivo.
int functionality7_sortFile(FILE *inputFile);

// Executa a funcionalidade 8 do Trabalho 3 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê dois arquivo binário, e salva um novo executando o merging entre eles.
int functionality8_fileMerging(FILE *inputFile);

// Executa a funcionalidade 9 do Trabalho 3 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê dois arquivo binário, e salva um novo executando o matching entre eles.
int functionality9_fileMatching(FILE *inputFile);

// Executa a funcionalidade 10 do Trabalho 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo binário e gera um arquivo de índice secundário.
int functionality10_createIndex(FILE *inputFile);

// Executa a funcionalidade 11 do Trabalho 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo de índice secundário e executa uma busca no arquivo principal.
int functionality11_searchWithIndex(FILE *inputFile);

// Executa a funcionalidade 14 do Trabalho 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Faz a comparação entre a eficiência da busca com e sem o indíce secundário.
int functionality14_searchCmp(FILE *inputFile);

# endif