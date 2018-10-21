#include "definitions.h"
#include "conversions.h"
#include "input.h"
#include "check_proof.h"

int main()
{
    int n;
    std::cout << "Enter the number of lines in the proof: ";
    std::cin >> n;
    std::cin.ignore();
    std::vector<Line> proof = input_proof(n);
    if(check_proof(proof))
        std::cout << "Valid proof.";
    else
        std::cout << "Invalid proof.";
    std::cout << std::endl;
    return 0;
}
