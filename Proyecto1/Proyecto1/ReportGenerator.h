#pragma once
#include <string>
#include <vector>
#include "Token.h"
#include "ErrorToken.h"


struct Paciente {
    std::string nombre;
    int edad = 0;
    std::string tipoSangre;
    int habitacion = 0;
    std::string condicion;
    std::string medicamento;
    std::string dosis;
};

struct Medico {
    std::string nombre;
    std::string especialidad;
    std::string codigo;
    int totalCitas = 0;
    int totalPacientesDistintos = 0;
};

struct Cita {
    std::string nombrePaciente;
    std::string nombreMedico;
    std::string fecha;
    std::string hora;
    std::string especialidadMedico;
    bool esConflicto = false;
};

struct Diagnostico {
    std::string nombrePaciente;
    std::string condicion;
    std::string medicamento;
    std::string dosis;
};


class ReportGenerator
{
    std::vector<Token> tokens;
    std::vector<ErrorToken> errors;
    std::vector<Paciente>    pacientes;
    std::vector<Medico>      medicos;
    std::vector<Cita>        citas;
    std::vector<Diagnostico> diagnosticos;


public:

	ReportGenerator();
    std::string Hospital = "";
    void generateReporte1(const std::string& outputPath);
    void generateReporte2(const std::string& outputPath);
    void generateReporte3(const std::string& outputPath);
    void generateReporte4(const std::string& outputPath);
    void generateGraphviz(const std::string& outputPath);
    void setTokens(const std::vector<Token>& tokens);
    void setErrors(const std::vector<ErrorToken>& errors);


    void parsearTokens(const std::vector<Token>& tokens);
    void cruzarDatos();

    std::string htmlHeader(const std::string& titulo, const std::string& extraCSS = "");
    void escribirArchivo(const std::string& path, const std::string& contenido);

	void graphvizToPNG(std::string& nombreArchivo);
};

