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
	vector<double> ValorVans;

	MI caminho;

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
	int Numero_geracoes = 1200;
	int Numero_individuos = 100;
	vector<Individuos> Populacao;
	MI AlunosEscola;
	vector<int> EscolaAluno;
	MD MatrizDistancias; // Aluno -  Van - Escola
	int NumeroVans;
	int NumeroAlunos;
	int NumeroEscolas;
	vector<double> FitxGen;
	double desvioPadraoEntreVans;
	MD FitxGenVans;
	int Mutacao = 10;
	int Cruzamento = 60;
	int peso = 0;
	vector<RetornaCaminho> Caminho;

	AlgoritmoGenetico(MadJ MatrizADJ);
	void cruzamento();
	void Ordenar();
	int SelecaoRanking();
	void Fitness();
	Individuos SubFitness(MI vans, MI NumeroDeAlunosPorEcola);
	double calculoDesvioPadrao(vector<double> vans);
	void Start();
	int SelecaoTorneio();
	void GraficosFIT();
	void tirazero();
	double desvioPadraoDasVans();
};
AlgoritmoGenetico::AlgoritmoGenetico(MadJ MatrizADJ)
{
	MatrizDistancias = MatrizADJ.matrizAdjacente;
	NumeroVans = MatrizADJ.qtdVan();
	NumeroAlunos = MatrizADJ.qtdAluno();
	NumeroEscolas = MatrizADJ.qtdEscola();
	AlunosEscola = MatrizADJ.AlunosPorEscola();
	EscolaAluno = MatrizADJ.retornaEscolaDoALuno();

	Populacao.resize(Numero_individuos);
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);
	for (auto population = 0; population < Populacao.size(); population++)
	{
		Populacao[population].Individuo.resize(NumeroAlunos);
		Populacao[population].ValorVans.resize(NumeroVans);
		for (auto i = 0; i < NumeroAlunos; i++)
		{								   //Indiv�duos aleat�rios n�o dopados.
			Populacao[population].Individuo[i] = i; // Caso a ordem for Aluno, van, escola.
			Populacao[population].Valor = 0;
		}

		shuffle(Populacao[population].Individuo.begin(), Populacao[population].Individuo.end(), default_random_engine((unsigned int)dist(mt)));
	}
}

void AlgoritmoGenetico::cruzamento()
{

	vector<Individuos> Populacaoulation;
	Populacaoulation.push_back(Populacao[Populacao.size() - 1]);
	Populacaoulation.push_back(Populacao[Populacao.size() - 2]);

	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);
	for (unsigned int i = 0; i < (Populacao.size() - 2) / 2; i++)
	{

		int pai1 = SelecaoTorneio();
		int pai2 = SelecaoTorneio();

		if ((int)dist(mt) % 101 <= Cruzamento)
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

			copy(Populacao[pai2].Individuo.begin() + p1, Populacao[pai2].Individuo.begin() + p2, filho2.Individuo.begin() + p1);

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

			if ((int)dist(mt) % 101 < Mutacao)
			{
				int m1 = (int)dist(mt) % (NumeroAlunos - 1);
				int m2 = p1 + (int)dist(mt) % (NumeroAlunos - p1);
				int aux = filho1.Individuo[m1];
				filho1.Individuo[m1] = filho1.Individuo[m2];
				filho1.Individuo[m2] = aux;
			}

			if ((int)dist(mt) % 101 < Mutacao)
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

int AlgoritmoGenetico::SelecaoTorneio()
{
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 1000.0);
	vector<int> selecionado;
	int random;
	random = (int)dist(mt) % Populacao.size();
	selecionado.resize(Populacao.size());
	int aux = static_cast<int> (Populacao.size() * 0.1);

	for (int rand = 0; rand < aux; rand++)
	{
		if (selecionado[random] == 0)
		{
			selecionado[random] = 1;
		}

		else
		{
			rand--;
		}
		random = (int)dist(mt) % Populacao.size();
	}
	double menor = 1000;
	int escolhido = -1;
	for (int rand = 0; rand < selecionado.size(); rand++)
	{
		if (selecionado[rand] == 1)
		{
			menor = Populacao[rand].Valor < menor ? Populacao[rand].Valor : menor;
			escolhido = rand;
		}
	}
	return escolhido;
}

int AlgoritmoGenetico::SelecaoRanking()
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
		MI NumeroDeAlunosPorEscolaDeCadaVan(NumeroVans);

		for (int tam = 0; tam < NumeroDeAlunosPorEscolaDeCadaVan.size(); ++tam) {
			NumeroDeAlunosPorEscolaDeCadaVan[tam].resize(NumeroEscolas);
		}
		MI AlunoDeCadaVan(vans.size());



		/*Adiciona o aluno em cada van*/
		for (auto Vans = 0; Vans < vans.size(); Vans++)
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
			//Populacao[population].caminho = individuoRetorno.caminho;
			Populacao[population].Valor = individuoRetorno.Valor;
			Populacao[population].ValorVans = individuoRetorno.ValorVans;
			AlunoDeCadaVan.clear();
		}

	}
	Ordenar();
}

Individuos AlgoritmoGenetico::SubFitness(MI vans, MI NumeroDeAlunosPorEscolaDeCadaVan)
{
	Individuos retorno;
	retorno.ValorVans.resize(vans.size());
	vector<int> prox;
	double fit = 0.0; 	

	for (auto van = 0; van < vans.size(); van++)
	{
		fit += MatrizDistancias[vans[van][0]][(NumeroAlunos + van)];
		NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][0]]]--;

		if (NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][0]]] == 0)
		{
			prox.push_back(EscolaAluno[vans[van][0]] + NumeroAlunos + NumeroVans);
			NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][0]]] = -1;
		}

		for (int caminho = 1; caminho < vans[van].size(); caminho++)
		{
			if (prox.size() == 0)															 // Se o vetor prox for nulo, quer dizer que não há escolas.
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
				double auxFit = MatrizDistancias[vans[van][(caminho - 1)]][vans[van][caminho]];
				int aux = -1;
				//double aux[2] = { MatrizDistancias[vans[van][caminho - 1]][vans[van][caminho]], -1 };
				for (int PossiveisEscolas = 0; PossiveisEscolas < prox.size(); PossiveisEscolas++)
				{
					if (auxFit > MatrizDistancias[vans[van][(caminho - 1)]][prox[PossiveisEscolas]])
					{
						auxFit = MatrizDistancias[vans[van][caminho - 1]][prox[PossiveisEscolas]];
						aux = PossiveisEscolas;
					}
				}
				if (aux == -1)
				{
					fit += auxFit;
					NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]]--;

					if (NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]] == 0)
					{
						prox.push_back(EscolaAluno[vans[van][caminho]] + NumeroAlunos + NumeroVans);
						NumeroDeAlunosPorEscolaDeCadaVan[van][EscolaAluno[vans[van][caminho]]] = -1;
					}
				}
				else
				{
					fit += MatrizDistancias[vans[van][caminho - 1]][prox[aux]];
					vans[van].insert(vans[van].begin() + caminho, prox[aux]);
					prox.erase(prox.begin() + (int) aux);
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
		vans[van].insert(vans[van].begin(), NumeroAlunos + van);

		retorno.ValorVans[van] = fit;
		fit = 0;
	}
	retorno.Valor = 0;
	for (auto van = 0; van < retorno.ValorVans.size(); van++)
	{
		retorno.Valor += retorno.ValorVans[van];
	}
	//retorno.caminho = vans;
	retorno.Valor += calculoDesvioPadrao(retorno.ValorVans) * peso;
	return retorno;

}

double AlgoritmoGenetico::calculoDesvioPadrao(vector<double> vans)
{
	double media = 0;
	double desvioPadrao = 0;

	for (auto van = 0; van < vans.size(); van++){
		
		media += vans[van];
	}

	media /= vans.size();

	for (auto van = 0; van < vans.size(); van++){
		
		desvioPadrao += pow((vans[van] - media), 2);
	}
	desvioPadrao /= vans.size();

	desvioPadrao = sqrt(desvioPadrao);
	return desvioPadrao;
}

void AlgoritmoGenetico::Ordenar()
{
	sort(Populacao.begin(), Populacao.end());

}

double AlgoritmoGenetico::desvioPadraoDasVans()
{
	return calculoDesvioPadrao((*(Populacao.end() - 1)).ValorVans);

}

void AlgoritmoGenetico::GraficosFIT()
{
	FitxGen.push_back((*(Populacao.end() - 1)).Valor);
	FitxGenVans.push_back((*(Populacao.end() - 1)).ValorVans);

	//RetornaCaminho retornodocaminho;
	//retornodocaminho.caminho = (*(Populacao.end() - 1)).caminho;
	//Caminho.push_back(retornodocaminho);
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

void AlgoritmoGenetico::Start()
{
	double aux = 1000;
	int contadorMutacao = 0;
	int mutacaoInicial = Mutacao;

	for (int i = 0; i < Numero_geracoes; ++i)
	{
		cruzamento();
		Fitness();
		Ordenar();
		GraficosFIT();

		if (i % 100 == 0)
		{
			RetornaCaminho retornodocaminho;
			retornodocaminho.caminho = (*(Populacao.end() - 1)).caminho;
			Caminho.push_back(retornodocaminho);
		}

		if (fabs(aux - *(FitxGen.end() - 1)) < 0.001)
		{
			contadorMutacao++;
			if (contadorMutacao == 20)
			{
				Mutacao *= (int) 1.5;
			}
		}
		else
		{
			contadorMutacao = 0;
			Mutacao = mutacaoInicial;
			aux = *(FitxGen.end() - 1);
		}



	}
	// Colocar a função pra fazer o grafico aqui
	Mutacao = mutacaoInicial;
}

void Inicia(int NumExecucoes)
{
	int Cruzamento = 0;
	int Mutacao = 0;
	int NumeroEscolas = 0;
	int	NumeroVans = 0;
	int	AC = 0;

	vector<double> DPdasVans;
	int pAC = 1; //numero que decide se o arquivo gerado é dos pontos aleatórios ou centrais. if(pAC==1)SÃO ALEATORIOS ||if(pAC!=1) SÃO CENTRAIS
	vector<int> mutacao;
	mutacao.reserve(24);
	//           |       1       |        2       |        3       |        4       |         
	int vec[] = { 5,10,20,40,60,80,5,10,20,40,60,80,5,10,20,40,60,80,5,10,20,40,60,80 };
	mutacao.insert(mutacao.begin(), vec, vec + 24);
	vector<int> cruzamento;
	cruzamento.reserve(24);
	//            |                |                 |                 |                 |
	int vec2[] = { 60,60,60,60,60,60,70,70,70,70,70,70,80,80,80,80,80,80,90,90,90,90,90,90 };
	cruzamento.insert(cruzamento.begin(), vec2, vec2 + 24);
	for (int peso = 0; peso <1; peso += 2) {

	for (int mudaNumeroDeAlunosVansEscolas = 4; mudaNumeroDeAlunosVansEscolas < 5; mudaNumeroDeAlunosVansEscolas++) {
		(mudaNumeroDeAlunosVansEscolas < 9) ? pAC = 1 : pAC = 2;
		
		MadJ* MatrizADJ = new MadJ(mudaNumeroDeAlunosVansEscolas);

		MatrizADJ->criaMadj(mudaNumeroDeAlunosVansEscolas);

		for (int MudaCruzamentoMutacao =23; MudaCruzamentoMutacao < 24; MudaCruzamentoMutacao++) {

			for (int execucao = 0; execucao < NumExecucoes; execucao++) {


				AlgoritmoGenetico* algoritmoGenetico = new AlgoritmoGenetico(*MatrizADJ);

				algoritmoGenetico->Cruzamento = cruzamento[MudaCruzamentoMutacao];
				algoritmoGenetico->Mutacao = mutacao[MudaCruzamentoMutacao];
				algoritmoGenetico->peso = peso;
				Cruzamento = algoritmoGenetico->Cruzamento;
				Mutacao = algoritmoGenetico->Mutacao;
				NumeroEscolas = algoritmoGenetico->NumeroEscolas;
				NumeroVans = algoritmoGenetico->NumeroVans;
				AC = pAC;
				algoritmoGenetico->Start();

				MatrizADJ->criaGraficoGeracaoFit(
					algoritmoGenetico->FitxGen,
					algoritmoGenetico->Caminho,
					algoritmoGenetico->Numero_geracoes,
					algoritmoGenetico->Numero_individuos,
					algoritmoGenetico->Cruzamento,
					algoritmoGenetico->Mutacao,
					algoritmoGenetico->NumeroEscolas,
					algoritmoGenetico->NumeroVans,
					execucao,
					pAC,
					peso,
					algoritmoGenetico->FitxGenVans);
				MatrizADJ->caminho(
					algoritmoGenetico->Caminho,
					algoritmoGenetico->Numero_geracoes,
					algoritmoGenetico->Numero_individuos,
					algoritmoGenetico->Cruzamento,
					algoritmoGenetico->Mutacao,
					algoritmoGenetico->NumeroEscolas,
					algoritmoGenetico->NumeroVans,
					execucao,
					pAC,
					peso);

					DPdasVans.push_back(algoritmoGenetico->desvioPadraoDasVans());

				delete algoritmoGenetico;
			}

			MadJ* b = new MadJ();
			b->criaArquivoDP(
				DPdasVans,
				1200,
				100,
				Cruzamento,
				Mutacao,
				NumeroEscolas,
				NumeroVans,
				AC,
				peso);
			DPdasVans.clear(); 
			//delete b;
		}
	}
	}
}

int main()
{
	Inicia(1); // Numero execuções
	return 0;
}