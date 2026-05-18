#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>

using Matriz = std::vector<std::vector<int>>;

Matriz multp_matrizes_seq(const Matriz &A, const Matriz &B){
    // Correção da validação: Colunas de A devem ser iguais às Linhas de B
    if(A[0].size() != B.size()) {
        std::cerr << "[ERRO] dimensões inválidas de matrizes" << std::endl;
        return {};
    }

    Matriz C(A.size(), std::vector<int>(B[0].size(), 0));

    for(size_t i = 0; i < A.size(); i++){
        for(size_t j = 0; j < B[0].size(); j++){
            for(size_t k = 0; k < B.size(); k++){
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }
    return C;
}

void vl_matriz_paralelo(const Matriz &A, const Matriz &B, Matriz &C, int inicio, int fim){
    for(int i = inicio; i < fim; i++)
        for(size_t j = 0; j < B[0].size(); j++)
            for(size_t k = 0; k < B.size(); k++)
                C[i][j] += A[i][k] * B[k][j];
}

Matriz multp_matrizes_paralelo(const Matriz &A, const Matriz &B, int num_threads = 0){
    if(A[0].size() != B.size()) {
        std::cerr << "[ERRO] dimensões inválidas de matrizes" << std::endl;
        return {};
    }
    
    Matriz C(A.size(), std::vector<int>(B[0].size(), 0));
    std::vector<std::thread> linesT;
    
    // Se num_threads for 0 ou maior que o número de linhas, limita ao número de linhas
    if(num_threads <= 0 || num_threads > static_cast<int>(A.size())){
        num_threads = A.size();
    }
    
    int qt_line_threads = A.size() / num_threads;
    int resto = A.size() % num_threads;
    int linha_atual = 0;
    
    for(int i = 0; i < num_threads; i++){
        int inicio = linha_atual;
        int fim = inicio + qt_line_threads + (i < resto ? 1 : 0);
        
        linesT.push_back(std::thread(vl_matriz_paralelo, std::ref(A), std::ref(B), std::ref(C), inicio, fim));
        linha_atual = fim;
    }
    
    for(auto& lT : linesT)
        if(lT.joinable())
            lT.join();

    return C;
}

int main(){
    std::stringstream result;
    std::vector<int> tamanhos = {100, 200, 400, 800, 1000}; 
    std::vector<int> qt_threads = {2, 4, 8, 16, 0};
    
    for(int i = 0 ; i < 10; i++) { // Alterado para 1 repetição para teste inicial
        for(auto& tamanho : tamanhos){
            Matriz A(tamanho, std::vector<int>(tamanho, 1));
            Matriz B(tamanho, std::vector<int>(tamanho, 1));
            
            result << std::string(20,'=') << "\n";
            result << "Processando matriz " << tamanho << " x " << tamanho << "\n\n";
            std::cout << "Processando matriz " << tamanho << " x " << tamanho << "\n\n";
            
            result << "Sequencial:\n";
            std::cout << "Sequencial:\n";
            
            auto inicio = std::chrono::high_resolution_clock::now();
            multp_matrizes_seq(A, B);
            auto fim = std::chrono::high_resolution_clock::now();
            
            std::chrono::duration<double> tempo_seq = fim - inicio;
            result << "Tempo Sequencial: " << tempo_seq.count() << " segundos.\n";
            std::cout << "Tempo Sequencial: " << tempo_seq.count() << " segundos.\n";
            result << std::string(15,'-') << "\n";
            
            result << "Paralelo:\n";
            std::cout << "Paralelo:\n";
            for(auto& n_threads : qt_threads){
                inicio = std::chrono::high_resolution_clock::now();
                multp_matrizes_paralelo(A, B, n_threads);
                fim = std::chrono::high_resolution_clock::now();
                
                std::chrono::duration<double> tempo_paralelo = fim - inicio;
                std::string msg_threads = n_threads == 0 ? "o máximo de" : std::to_string(n_threads);
                
                result << "Tempo Paralelo com " << msg_threads << " threads: " << tempo_paralelo.count() << " segundos.\n";
                std::cout << "Tempo Paralelo com " << msg_threads << " threads: " << tempo_paralelo.count() << " segundos.\n";
            }
            result << std::string(15,'-') << "\n";
        }
    }

    std::ofstream resultado("output.txt");
    if(resultado.is_open()){
        resultado << result.str();
        resultado.close();
        std::cout << "\n[SUCESSO] Arquivo output.txt gerado com sucesso!" << std::endl;
    } else {
        std::cerr << "\n[ERRO] Não foi possível abrir o arquivo output.txt" << std::endl;
    }
    
    return EXIT_SUCCESS;
}