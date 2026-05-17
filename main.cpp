#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>

using Matriz = std::vector<std::vector<int>>;

Matriz multp_matrizes_seq(Matriz A, Matriz B){
    Matriz C(A.size(), std::vector<int>(B[0].size(), 0));

    if(A.size() != B[0].size()) std::cerr << "[ERRO] dimensões ínvalidas de matrizes";
    for(int i = 0; i < A.size(); i++){
        for(int j = 0; j < B[0].size(); j++){
            for(int k = 0; k < B.size(); k++){
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }
    return C;
}

void print_matriz(Matriz M){
    std::cout << "[" << std::endl;
    for(auto&linha : M){
        std::cout << "[ ";
        for(auto&vl : linha) 
            std::cout << vl << " ";
        std::cout << "]" << std::endl;
    }
    std::cout << "]" << std::endl;
}

void vl_matriz_paralelo(Matriz &A, Matriz &B, Matriz &C, int inicio, int fim){
    for(int i = inicio; i < fim; i++)
        for(int j = 0; j < B[0].size(); j++)
            for(int k = 0; k < B.size(); k++)
                C[i][j] += A[i][k] * B[k][j];
}

Matriz multp_matrizes_paralelo(Matriz A, Matriz B, int num_threads=0){
    if(A[0].size() != B.size()) std::cerr << "[ERRO] dimensões ínvalidas de matrizes";
    
    Matriz C(A.size(), std::vector<int>(B[0].size(), 0));
    std::vector<std::thread> linesT;
    int 
        qt_line_threads = 1, resto=0, linha_atual = 0,
        inicio, fim;
    
    if(num_threads != 0){
        num_threads = std::min(num_threads, static_cast<int>(A.size()));
        qt_line_threads = A.size()/num_threads;
        resto = A.size()%num_threads;
    }else
        num_threads = A.size();
    
    for(int i = 0; i < num_threads; i++){
        inicio = linha_atual;
        fim = inicio+qt_line_threads+(i < resto ? 1 : 0);
        linesT.push_back(std::thread(vl_matriz_paralelo, std::ref(A), std::ref(B), std::ref(C),inicio, fim));
        linha_atual = fim;
    }
    
    for(auto&lT :linesT)
        if(lT.joinable())
            lT.join();
    return C;
}

int main(){
    std::stringstream result;
    std::vector<int> tamanhos = {1000,2000,4000,8000,10000};
    std::vector<int> qt_threads = {2,4,8,16,0};
    
    for(int i = 0 ; i < 10; i++)
        for(auto& tamanho : tamanhos){
            Matriz A(tamanho, std::vector<int>(tamanho,1));
            Matriz B(tamanho, std::vector<int>(tamanho,1));
            
            result << std::string(20,'=') << std::endl;
            result << "Processando matriz " << tamanho << " x " << tamanho << std::endl << std::endl;
            std::cout << "Processando matriz " << tamanho << " x " << tamanho << std::endl << std::endl;
            result << "Sequencial:\n";
            auto inicio = std::chrono::high_resolution_clock::now();
            multp_matrizes_seq(A,B);
            auto fim = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> tempo_seq = fim - inicio;
            result << "Tempo Sequencial: " << tempo_seq.count() << " segundos.\n";
            result << std::string(15,'-') << std::endl;
            
            result << "Paralelo:\n";
            for(auto& n_threads : qt_threads){
                inicio = std::chrono::high_resolution_clock::now();
                multp_matrizes_paralelo(A,B,n_threads);
                fim = std::chrono::high_resolution_clock::now();
                tempo_seq = fim - inicio;
                std::string msg_threads = n_threads == 0 ? "o máximo de" : std::to_string(n_threads);
                result << "Tempo Paralelo com  " << msg_threads << " threads: " << tempo_seq.count() << " segundos.\n";
                result << std::string(15,'-') << std::endl;
            }
        }

    std::ofstream resultado("output.txt");
    if(resultado.is_open()){
        resultado << result.str();
        resultado.close();
    }
    return EXIT_SUCCESS;
}