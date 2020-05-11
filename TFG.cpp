#include <string>
#include <algorithm>
#include <string.h>
#include <vector>
#include <limits>
#include <random>
#include <iostream>
#include <cstdlib>
#include "criaMatrizAdjC.h"

using namespace std;

typedef vector<vector<int>> MI;
typedef vector<vector<double>> MD;


struct Individuos
{
	double Valor = 0;
	vector<int> Individuo;
	vector<vector<int>> caminho;

	Individuos(double a)
	{
		Valor = a;
	}
	Individuos() {}
	bool operator<(const Individuos& Comparar) const
	{
		return Valor > Comparar.Valor;
	}
	bool operator==(const Individuos& Comparar) const
	{
		return Valor == Comparar.Valor;
	}
	bool operator!=(const Individuos& Comparar) const
	{
		return Valor != Comparar.Valor;
	}
};

class AlgoritmoGenetico
{
public:
	int Numero_geracoes = 500;
	int Numero_individuos = 100;
	vector<Individuos> Populacao;
	MI AlunosEscola;
	vector<int> EscolaAluno;
	MD MatrizDistancias; // Aluno -  Van - Escola
	int NumeroVans;
	int NumeroAlunos;
	int NumeroEscolas;
	vector<double> FitxGen;
	int Mutacao = 20;
	int Cruzamento = 60;
	vector<RetornaCaminho> Caminho;

	AlgoritmoGenetico(int numeroDoArquivo);
	void cruzamento();
	void Ordenar();
	int Selecao();
	void Fitness();
	Individuos SubFitness(vector<vector<int>> vans, vector<vector<int>> NumeroDeAlunosPorEcola);
	void Start();
	void GraficosFIT();
	void tirazero();
	MadJ* MatrizADJ;
};
AlgoritmoGenetico::AlgoritmoGenetico(int numeroDoArquivo)
{
	this->MatrizADJ = new MadJ(numeroDoArquivo);
	MatrizDistancias = MatrizADJ->matrizAdjacente;
	NumeroVans = MatrizADJ->qtdVan();
	NumeroAlunos = MatrizADJ->qtdAluno();
	NumeroEscolas = MatrizADJ->qtdEscola();
	AlunosEscola = MatrizADJ->AlunosPorEscola();
	EscolaAluno = MatrizADJ->retornaEscolaDoALuno();

	Populacao.resize(Numero_individuos);
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);
	for (unsigned int a = 0; a < Populacao.size(); a++)
	{
		Populacao[a].Individuo.resize(NumeroAlunos); // Inicialmente com lixo.
		for (auto i = 0; i < NumeroAlunos; i++)
		{								   //Indiv�duos aleat�rios n�o dopados.
			Populacao[a].Individuo[i] = i; // Caso a ordem for Aluno, van, escola.
			Populacao[a].Valor = 0;
		}

		shuffle(Populacao[a].Individuo.begin(), Populacao[a].Individuo.end(), default_random_engine((unsigned int)dist(mt)));
	}
}
Individuos AlgoritmoGenetico::SubFitness(vector<vector<int>> vans, vector<vector<int>> NumeroDeAlunosPorEscolaDeCadaVan)
{
	Individuos retorno;
	vector<int> prox;
	double fit = 0.0;

	for (int van = 0; van < vans.size(); van++)
	{
		fit += MatrizDistancias[vans[van][0]][NumeroAlunos + van];
		NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][0]]]--;

		if (NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][0]]] == 0)
		{
			prox.push_back(EscolaAluno[vans[van][0]] + NumeroAlunos + NumeroVans);
			NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][0]]] = -1;
		}

		for (int caminho = 1; caminho < vans[van].size(); caminho++)
		{
			if (prox.size() == 0)												 // Se o vetor prox for nulo, quer dizer que não há escolas.
			{
				fit += MatrizDistancias[vans[van][caminho - 1]][vans[van][caminho]]; //NumeroAlunos + van corresponde ao número da van que esta na matriz de adjacência.

				NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]]--;

				if (NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]] == 0)
				{
					prox.push_back(EscolaAluno[vans[van][caminho]] + NumeroAlunos + NumeroVans);
					NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]] = -1;
				}
			}
			else
			{
				double aux[2] = { MatrizDistancias[vans[van][caminho - 1]][vans[van][caminho]], -1 };
				for (int PossiveisEscolas = 0; PossiveisEscolas < prox.size(); PossiveisEscolas++)
				{
					if (aux[0] > MatrizDistancias[vans[van][caminho - 1]][prox[PossiveisEscolas]])
					{
						aux[0] = MatrizDistancias[vans[van][caminho - 1]][prox[PossiveisEscolas]];
						aux[1] = PossiveisEscolas;
					}
				}
				if (aux[1] == -1)
				{
					fit += aux[0];
					NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]]--;

					if (NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]] == 0)
					{
						prox.push_back(EscolaAluno[vans[van][caminho]] + NumeroAlunos + NumeroVans);
						NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]] = -1;
					}
				}
				else
				{
					fit += MatrizDistancias[vans[van][caminho - 1]][prox[aux[1]]];
					vans[van].insert(vans[van].begin() + caminho, prox[aux[1]]);
					prox.erase(prox.begin() + aux[1]);
				}
			}
			if (caminho == vans[van].size() - 1 && prox.size() != 0)
			{
				fit += MatrizDistancias[vans[van][caminho]][prox[0]];
				vans[van].insert(vans[van].begin() + caminho + 1, prox[0]);
				prox.clear();
				caminho++;
			}
		}


	}
	retorno.Valor = fit;
	retorno.caminho = vans;
	return retorno;
}
void AlgoritmoGenetico::tirazero()
{
	for (unsigned int i = 0; i < MatrizDistancias.size(); i++)
	{
		MatrizDistancias[i][i] = 500;
	}
}
vector<int> divide(vector<int> sub, int i, int j)
{
	vector<int> aux(sub.begin() + i, sub.begin() + j);
	return aux;
}

void AlgoritmoGenetico::Fitness()
{
	Individuos individuoRetorno;
	int TamMin = NumeroAlunos / NumeroVans;
	int aux = 0;
	vector<int> vans;
	vans.resize(NumeroVans);
	for (int i = 0; i < NumeroVans; i++)
	{
		vans[i] = ((i + 1) * TamMin);
	}

	if (NumeroAlunos % NumeroVans != 0)
	{
		for (int i = 0; i < NumeroVans; i++)
			if ((NumeroAlunos % NumeroVans) > i)
			{
				vans[i] += (i + 1);
				aux = i + 1;
			}
			else
			{
				vans[i] += aux;
			}
	}

	for (int population = 0; population < Populacao.size(); population++)
	{
		vector<vector<int>> NumeroDeAlunosPorEscolaDeCadaVan(NumeroVans);

		for (int tam = 0; tam < NumeroDeAlunosPorEscolaDeCadaVan.size(); ++tam) {
			NumeroDeAlunosPorEscolaDeCadaVan[tam].resize(NumeroEscolas);
		}
		vector<vector<int>> AlunoDeCadaVan(vans.size());



		/*Adiciona o aluno em cada van*/
		for (int Vans = 0; Vans < vans.size(); Vans++)
		{
			for (int Alunos = Vans == 0 ? 0 : vans[Vans - 1]; Alunos < vans[Vans]; Alunos++)
			{
				AlunoDeCadaVan[Vans].push_back(Populacao[population].Individuo[Alunos]);
			}
		}
		/* Coloca quantos alunos tem em cada escola para cada van (O vetor é Van por escola, e nos índices contém quantos alunos possuem daquela escola)*/
		for (int NumeroDaVan = 0; NumeroDaVan < NumeroVans; NumeroDaVan++)
		{
			for (int Escolas = 0; Escolas < NumeroEscolas; Escolas++)
			{
				for (int individuos = 0; individuos < AlunoDeCadaVan[NumeroDaVan].size(); individuos++)
				{

					if (find(AlunosEscola[Escolas].begin(), AlunosEscola[Escolas].end(), AlunoDeCadaVan[NumeroDaVan][individuos]) != AlunosEscola[Escolas].end())
						NumeroDeAlunosPorEscolaDeCadaVan[NumeroDaVan][Escolas]++;
				}
			}
		}
		/*Coloca -1 nas escolas que não possuem nenhum aluno atribuido a ela*/
		for (int escolas = 0; escolas < NumeroDeAlunosPorEscolaDeCadaVan.size(); ++escolas)
		{
			for (int alunos = 0; alunos < NumeroDeAlunosPorEscolaDeCadaVan[escolas].size(); alunos++)
			{
				if (NumeroDeAlunosPorEscolaDeCadaVan[escolas][alunos] == 0)
				{
					NumeroDeAlunosPorEscolaDeCadaVan[escolas][alunos] = -1;
				}
			}
		}

		if (Populacao[population].Valor == 0)
		{
			individuoRetorno = SubFitness(AlunoDeCadaVan, NumeroDeAlunosPorEscolaDeCadaVan);
			Populacao[population].caminho = individuoRetorno.caminho;
			Populacao[population].Valor = individuoRetorno.Valor;
			AlunoDeCadaVan.clear();
		}

	}
	Ordenar();
}

void AlgoritmoGenetico::Ordenar()
{
	sort(Populacao.begin(), Populacao.end());

}

int AlgoritmoGenetico::Selecao()
{
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);

	int sorteio = (int)dist(mt) % 101;
	double aux = 0;
	for (unsigned int i = 1; i < Populacao.size(); ++i)
	{

		aux += (double)i / (((Populacao.size() + 1) * Populacao.size()) / 2);
		if (sorteio < aux * 100)
		{

			return i - 1;
		}
	}
	return 0;
}

void AlgoritmoGenetico::cruzamento()
{

	vector<Individuos> Populacaoulation;
	Populacaoulation.push_back(Populacao[Populacao.size() - 1]);
	Populacaoulation.push_back(Populacao[Populacao.size() - 1]);
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);
	for (unsigned int i = 0; i < (Populacao.size() - 2) / 2; i++)
	{

		int pai1 = Selecao();
		int pai2 = Selecao();

		if ((unsigned int)dist(mt) % 101 <= Cruzamento)
		{

			Individuos filho1;
			Individuos filho2;

			int p1 = (int)dist(mt) % (NumeroAlunos - 1);
			int p2 = p1 + (int)dist(mt) % (NumeroAlunos - p1);
			if (p1 == p2)
				++p2;

			filho1.Individuo.resize(NumeroAlunos, -1);
			filho2.Individuo.resize(NumeroAlunos, -1);
			copy(Populacao[pai1].Individuo.begin() + p1, Populacao[pai1].Individuo.begin() + p2, filho1.Individuo.begin() + p1);
			filho1.Individuo.size();
			copy(Populacao[pai2].Individuo.begin() + p1, Populacao[pai2].Individuo.begin() + p2, filho2.Individuo.begin() + p1);
			filho2.Individuo.size();
			int j = p2;
			for (int i = p2; j != p1; i = (i == filho1.Individuo.size() - 1) ? 0 : i + 1)
			{

				if (find(filho1.Individuo.begin(), filho1.Individuo.end(), Populacao[pai2].Individuo[i]) == filho1.Individuo.end())
				{
					filho1.Individuo[j] = Populacao[pai2].Individuo[i];
					j = (j != filho1.Individuo.size() - 1) ? j + 1 : 0;
				}
			}

			j = p2;
			for (int i = p2; j != p1; i = (i == filho2.Individuo.size() - 1) ? 0 : i + 1)
			{

				if (find(filho2.Individuo.begin(), filho2.Individuo.end(), Populacao[pai1].Individuo[i]) == filho2.Individuo.end())
				{
					filho2.Individuo[j] = Populacao[pai1].Individuo[i];
					j = (j != filho2.Individuo.size() - 1) ? j + 1 : 0;
				}
			}
			if ((unsigned int)dist(mt) % 101 < Mutacao)
			{
				int m1 = (int)dist(mt) % (NumeroAlunos - 1);
				int m2 = p1 + (int)dist(mt) % (NumeroAlunos - p1);
				int aux = filho1.Individuo[m1];
				filho1.Individuo[m1] = filho1.Individuo[m2];
				filho1.Individuo[m2] = aux;
			}
			if ((unsigned int)dist(mt) % 101 < Mutacao)
			{
				int m1 = (int)dist(mt) % filho1.Individuo.size();
				int m2 = (int)dist(mt) % filho2.Individuo.size();
				int aux = filho2.Individuo[m1];
				filho2.Individuo[m1] = filho2.Individuo[m2];
				filho2.Individuo[m2] = aux;
			}

			Populacaoulation.push_back(filho1);
			Populacaoulation.push_back(filho2);
		}

		else
		{
			Populacaoulation.push_back(Populacao[pai1]);
			Populacaoulation.push_back(Populacao[pai2]);
		}
	}

	Populacao = Populacaoulation;

	for (int a = 2; a < Populacao.size(); a++)
	{
		Populacao[a].Valor = 0;
		Populacao[a].caminho.clear();
	}

}

void AlgoritmoGenetico::GraficosFIT()
{
	FitxGen.push_back((*(Populacao.end() - 1)).Valor);
	RetornaCaminho retornodocaminho;
	retornodocaminho.caminho = (*(Populacao.end() - 1)).caminho;
	Caminho.push_back(retornodocaminho);
}

void AlgoritmoGenetico::Start()
{
	for (int i = 0; i < Numero_geracoes; ++i)
	{
		cruzamento();
		Fitness();
		Ordenar();
		GraficosFIT();
	}
	// Colocar a função pra fazer o grafico aqui

}
void Inicia(int NumExecucoes)
{
	vector<int> mutacao;
	mutacao.resize(8);
	int vec[] = { 5,10,20,40,60,5,5,5 };
	mutacao.insert(mutacao.begin(), vec, vec + 8);
	vector<int> cruzamento;
	cruzamento.resize(8);
	int vec2[] = { 60, 60, 60, 60, 60, 70, 80, 90 };
	cruzamento.insert(cruzamento.begin(), vec2, vec2 + 8);
	for (int mudaNumeroDeAlunosVansEscolas = 1; mudaNumeroDeAlunosVansEscolas < 5; ++mudaNumeroDeAlunosVansEscolas) {

		for (int MudaCruzamentoMutacao = 0; MudaCruzamentoMutacao < 8; MudaCruzamentoMutacao++) {

			for (int execucao = 0; execucao < NumExecucoes; execucao++) {
				AlgoritmoGenetico* a = new AlgoritmoGenetico(mudaNumeroDeAlunosVansEscolas);
				a->Cruzamento = cruzamento[MudaCruzamentoMutacao];
				a->Mutacao = mutacao[MudaCruzamentoMutacao];
				a->Start();
				a->MatrizADJ->criaGraficoGeracaoFit(
					a->FitxGen,
					a->Caminho,
					a->Numero_geracoes,
					a->Numero_individuos,
					a->Cruzamento,
					a->Mutacao,
					a->NumeroEscolas,
					a->NumeroVans,
					execucao);

				delete a;
			}
		}
	}
}

int main()
{
	Inicia(30); // Numero execuções
	return 0;
}