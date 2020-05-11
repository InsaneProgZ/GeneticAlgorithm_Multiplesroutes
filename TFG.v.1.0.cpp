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
	vector<double> ValorVans;
	vector<int> Individuo;

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
	vector<vector<double>> FitxGen; // 0 -  MELHOR ; 1 - média ; 2 - Pior
	vector<vector<double>> FitVans;
	vector<vector<int>> NumeroDeAlunosPorEscola;
	//bool Heuristic = 1;

	unsigned int Mutacao = 20;
	unsigned int Cruzamento = 60;

	AlgoritmoGenetico(int numeroDoArquivo);
	void cruzamento();
	void Ordenar();
	int Selecao();
	void Fitness();
	void SubFitness(vector<vector<int>> vans, vector<vector<int>> NumeroDeAlunosPorEcola);
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
	AlunosEscola = MatrizADJ->AlunosPorEscola(); //
	EscolaAluno = MatrizADJ->retornaEscolaDoALuno();

	Populacao.resize(Numero_individuos);
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);
	for (unsigned int a = 0; a < Populacao.size(); a++)
	{
		Populacao[a].ValorVans.resize(NumeroVans);
		Populacao[a].Individuo.resize(NumeroAlunos); // Inicialmente com lixo.
		for (auto i = 0; i < NumeroAlunos; i++)
		{                                  //Indiv�duos aleat�rios n�o dopados.
			Populacao[a].Individuo[i] = i; // Caso a ordem for Aluno, van, escola.
			Populacao[a].Valor = 0;
		}

		shuffle(Populacao[a].Individuo.begin(), Populacao[a].Individuo.end(), default_random_engine((unsigned int)dist(mt)));
	}
}
void AlgoritmoGenetico::SubFitness(vector<vector<int>> vans , vector<vector<int>> NumeroDeAlunosPorEcola)
{
	vector<int> prox;
	double fit = 0.0;












	/*
	vector<int> prox;
	vector<vector<int>> NumeroDeAlunosPorEscola;
	NumeroDeAlunosPorEscola.resize(NumeroVans);
	for (unsigned int tam = 0; tam < NumeroDeAlunosPorEscola.size(); ++tam)
		NumeroDeAlunosPorEscola[tam].resize(NumeroEscolas);

	double fit = 0.0;
	for (unsigned int i = 0; i < Populacao.size(); i++)
	{
		if (Populacao[i].Valor == 0)
		{
			for (unsigned int j = 0; j < vans.size(); j++)
			{

				if (j == 0)
				{
					for (int Ind = 0; Ind < vans[0]; Ind++)
					{
						for (unsigned int Escolas = 0; Escolas < NumeroEscolas; Escolas++)
						{
							if (find(AlunosEscola[Escolas].begin(), AlunosEscola[Escolas].end(),
								Populacao[i].Individuo[Ind]) != AlunosEscola[Escolas].end())
								NumeroDeAlunosPorEscola[j][Escolas]++;
						}
					} // ARRUMAR PARA CADA VAN



					for (int AluPY = 0; AluPY < NumeroEscolas; ++AluPY)
					{
						if (NumeroDeAlunosPorEscola[j][AluPY] == 0)
							NumeroDeAlunosPorEscola[j][AluPY] = -1;
					}


					fit += MatrizDistancias[NumeroAlunos][Populacao[i].Individuo[0]];
					if (NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[0]]] != -1)
						NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[0]]]--;
					for (unsigned int alun = 0; alun < NumeroDeAlunosPorEscola[j].size(); alun++)
					{
						if (NumeroDeAlunosPorEscola[j][alun] == 0)
						{
							NumeroDeAlunosPorEscola[j][alun] = -1;
							prox.push_back(AlunosEscola[alun][0]);
						}
					}

					for (int l = 1; l <= vans[j]; l++)
					{
						if (prox.size() == 0)
						{
							fit += MatrizDistancias[Populacao[i].Individuo[l - 1]][Populacao[i].Individuo[l]];
							if (NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]] != -1)
								NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]]--;
							for (unsigned int alun = 0; alun < NumeroDeAlunosPorEscola[j].size(); alun++)
							{
								if (NumeroDeAlunosPorEscola[j][alun] == 0)
								{
									NumeroDeAlunosPorEscola[j][alun] = -1;
									prox.push_back(AlunosEscola[alun][0]);
								}
							}
						}
						else
						{

							unsigned int aux = 0;
							for (unsigned int cam = 0; cam < prox.size(); cam++)
							{

								if (MatrizDistancias[Populacao[i].Individuo[l - 1]][prox[cam]] <
									MatrizDistancias[Populacao[i].Individuo[l - 1]][Populacao[i].Individuo[l]])
									aux = cam;
							}
							fit += MatrizDistancias[Populacao[i].Individuo[l - 1]][prox[aux]];
							fit += MatrizDistancias[prox[aux]][Populacao[i].Individuo[l]];
							prox.erase(prox.begin() + aux);
							if (NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]] != -1)
								NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]]--;

							for (unsigned int alun = 0; alun < NumeroDeAlunosPorEscola[j].size(); alun++)
							{
								if (NumeroDeAlunosPorEscola[j][alun] == 0)
								{

									NumeroDeAlunosPorEscola[j][alun] = -1;
									prox.push_back(AlunosEscola[alun][0]);
								}
							}
						}
					}
					Populacao[i].ValorVans[j] = fit;
					Populacao[i].Valor += fit;
					fit = 0;
				}
				else
				{
					for (int Ind = vans[j - 1]; Ind < vans[j]; Ind++)
					{
						for (unsigned int Escolas = 0; Escolas < AlunosEscola.size(); Escolas++)
						{
							if (find(AlunosEscola[Escolas].begin(), AlunosEscola[Escolas].end(),
								Populacao[i].Individuo[Ind]) != AlunosEscola[Escolas].end())
								NumeroDeAlunosPorEscola[j][Escolas]++;
						}
					} // ARRUMAR PARA CADA VAN




					for (int AluPY = 0; AluPY < NumeroEscolas; ++AluPY)
					{
						if (NumeroDeAlunosPorEscola[j][AluPY] == 0)
							NumeroDeAlunosPorEscola[j][AluPY] = -1;
					}


					fit += MatrizDistancias[NumeroAlunos + j][Populacao[i].Individuo[vans[j - 1]]];
					NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[j - 1]]]--;
					for (unsigned int alun = 0; alun < NumeroDeAlunosPorEscola[j].size(); alun++)
					{
						if (NumeroDeAlunosPorEscola[j][alun] == 0)
						{
							NumeroDeAlunosPorEscola[j][alun] = -1;
							prox.push_back(AlunosEscola[alun][0]);
						}
					}
					for (int l = vans[j - 1] + 1; l < vans[j]; ++l)
					{


						if (prox.size() == 0)
						{
							fit += MatrizDistancias[Populacao[i].Individuo[l - 1]][Populacao[i].Individuo[l]];
							if (NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]] != -1)
								NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]]--;
							for (unsigned int alun = 0; alun < NumeroDeAlunosPorEscola[j].size(); alun++)
							{
								if (NumeroDeAlunosPorEscola[j][alun] == 0)
								{
									NumeroDeAlunosPorEscola[j][alun] = -1;
									prox.push_back(AlunosEscola[alun][0]);
								}
							}
						}

						else
						{

							int aux = 0;
							for (unsigned int cam = 0; cam < prox.size(); cam++)
							{

								if (MatrizDistancias[Populacao[i].Individuo[l - 1]][prox[cam]] <
									MatrizDistancias[Populacao[i].Individuo[l - 1]][Populacao[i].Individuo[l]])
									aux = cam;
							}
							fit += MatrizDistancias[Populacao[i].Individuo[l - 1]][prox[aux]];
							fit += MatrizDistancias[prox[aux]][Populacao[i].Individuo[l]];
							prox.erase(prox.begin() + aux);
							if (NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]] != -1)
								NumeroDeAlunosPorEscola[j][EscolaAluno[Populacao[i].Individuo[l]]]--;

							for (unsigned int alun = 0; alun < NumeroDeAlunosPorEscola[j].size(); alun++)
							{
								if (NumeroDeAlunosPorEscola[j][alun] == 0)
								{
									NumeroDeAlunosPorEscola[j][alun] = -1;
									prox.push_back(AlunosEscola[alun][0]);
								}
							}
						}
					}
					Populacao[i].ValorVans[j] = fit;
					Populacao[i].Valor += fit;
					fit = 0;
				}
			}
		}
	}*/
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
	int TamMin = NumeroAlunos / NumeroVans;
	vector<int> vans;
	vans.resize(NumeroVans);
	for (int i = 0; i < NumeroVans; i++)
		vans[i] = ((i + 1) * TamMin);

	int aux = 0;
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


	//SubFitness(vans);
	
	vector<vector<int>> NumeroDeAlunosPorEscola(NumeroVans);

	for (int tam = 0; tam < NumeroDeAlunosPorEscola.size(); ++tam) {
		NumeroDeAlunosPorEscola[tam].resize(NumeroEscolas);
	}
	vector<vector<int>> AlunoDeCadaVan(vans.size());

	for (int population = 0; population < Populacao.size(); population++)
	{
		
		/* Coloca quantos alunos tem em cada escola para cada van (O vetor é Van por escola, e nos índices contém quantos alunos possuem daquela escola)*/
		for (int NumeroDaVan = 0; NumeroDaVan < NumeroVans; NumeroDaVan ++ )
		{
			for (int Ind = 0; Ind < vans[0]; Ind++)
			{
				for (unsigned int Escolas = 0; Escolas < NumeroEscolas; Escolas++)
				{
					if (find(AlunosEscola[Escolas].begin(), AlunosEscola[Escolas].end(), Populacao[population].Individuo[Ind]) != AlunosEscola[Escolas].end())
						NumeroDeAlunosPorEscola[NumeroDaVan][Escolas]++;
				}
			}
		}
		/*Coloca -1 nas escolas que não possuem nenhum aluno atribuido a ela*/
		for (int escolas = 0; escolas < NumeroDeAlunosPorEscola.size(); ++escolas)
		{
			for (int alunos = 0; alunos < NumeroDeAlunosPorEscola[escolas].size(); alunos++) 
			{
				if (NumeroDeAlunosPorEscola[escolas][alunos] == 0)
				{
					NumeroDeAlunosPorEscola[escolas][alunos] = -1;
				}
			}
		}
				

		/*Adiciona o aluno de cada van*/
		for (int Vans = 0; Vans < vans.size(); Vans++)
		{
			for (int Alunos = Vans == 0 ? 0 : vans[Vans-1]; Alunos < vans[Vans] ; Alunos++)
			{
				AlunoDeCadaVan[Vans].push_back(Populacao[population].Individuo[Alunos]);
			}
		}	
		SubFitness(AlunoDeCadaVan, NumeroDeAlunosPorEscola);

	} 


}

void AlgoritmoGenetico::Ordenar()
{
	sort(Populacao.begin(), Populacao.end());
}

int AlgoritmoGenetico::Selecao()
{
	Ordenar(); // decrescente
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
			std::copy(Populacao[pai1].Individuo.begin() + p1, Populacao[pai1].Individuo.begin() + p2, filho1.Individuo.begin() + p1);
			filho1.Individuo.size();
			std::copy(Populacao[pai2].Individuo.begin() + p1, Populacao[pai2].Individuo.begin() + p2, filho2.Individuo.begin() + p1);
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
	for (unsigned int a = 2; a < Populacao.size(); a++)
	{
		Populacao[a].ValorVans.resize(NumeroVans);
		Populacao[a].Individuo.resize(NumeroAlunos); // Inicialmente com lixo.
		for (auto i = 0; i < NumeroAlunos; i++)
			Populacao[a].Valor = 0;

	}
}

void AlgoritmoGenetico::GraficosFIT()
{
	FitxGen.resize(3);
	FitxGen[0].push_back((*(Populacao.end() - 1)).Valor);
	FitxGen[1].push_back((*(Populacao.end() - 1)).Valor);
	for (unsigned int j = 0; j < Populacao.size() - 1; ++j)
	{
		*(FitxGen[1].end() - 1) += Populacao[j].Valor;
	}
	*(FitxGen[1].end() - 1) /= Populacao.size();
	FitxGen[2].push_back((*(Populacao.begin())).Valor);

	FitVans.resize(NumeroVans);
	for (int l = 0; l < NumeroVans; ++l)
	{
		FitVans[l].push_back((*(Populacao.end() - 1)).ValorVans[l]);
	}
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
				//a->MatrizADJ->criaGraficoGeracaoFit(
				//	a->FitxGen,
				//	a->Numero_geracoes,
				//	a->Numero_individuos,
				//	a->Cruzamento,
				//	a->Mutacao,
				//	a->NumeroEscolas,
				//	a->NumeroVans,
				//	execucao);

				//a->MatrizADJ->criaGraficoFitDeVanXGeracao(
				//	a->FitVans,
				//	a->Numero_geracoes,
				//	a->Numero_individuos,
				//	a->Cruzamento,
				//	a->Mutacao,
				//	a->NumeroEscolas,
				//	a->NumeroVans,
				//	execucao);
				delete a;
			}
		}
	}

}

int main()
{
	Inicia(30); // Numero execu��es


	return 0;
}
