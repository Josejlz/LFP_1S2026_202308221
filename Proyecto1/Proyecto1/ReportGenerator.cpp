#include "ReportGenerator.h"
#include "Token.h"
#include "ErrorToken.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <map>

ReportGenerator::ReportGenerator() {
}

static std::string sinComillas(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

void ReportGenerator::escribirArchivo(const std::string& path, const std::string& contenido) {
    std::ofstream archivo(path);
    if (archivo.is_open()) {
        archivo << contenido;
        archivo.close();
        std::cout << "Reporte generado con exito en: " << path << std::endl;
    }
    else {
        std::cerr << "Error: No se pudo abrir el archivo para escribir en: " << path << std::endl;
    }
}

void ReportGenerator::parsearTokens(const std::vector<Token>& tokens) {
    int i = 0;
    int n = (int)tokens.size();

    while (i < n) {
        std::string lex = tokens[i].lexeme;

        // ── para pacientes
        if (lex == "paciente" && i + 1 < n && tokens[i + 1].lexeme == ":") {
            Paciente p;
			i += 2; // para saltar "paciente" y ":"
            if (i < n) { p.nombre = sinComillas(tokens[i].lexeme); i++; }

			// Busqueda de atributos entre corchetes
            while (i < n && tokens[i].lexeme != "[") i++;
            i++; // saltar "["

            while (i < n && tokens[i].lexeme != "]") {
                std::string attr = tokens[i].lexeme;
                if (attr == "edad" && i + 2 < n) {
                    p.edad = std::stoi(tokens[i + 2].lexeme);
                    i += 3;
                }
                else if (attr == "tipo_sangre" && i + 2 < n) {
                    p.tipoSangre = sinComillas(tokens[i + 2].lexeme);
                    i += 3;
                }
                else if (attr == "habitacion" && i + 2 < n) {
                    p.habitacion = std::stoi(tokens[i + 2].lexeme);
                    i += 3;
                }
                else {
                    i++;
                }
            }
            pacientes.push_back(p);
        }

        // ── para medicos
        else if (lex == "medico" && i + 1 < n && tokens[i + 1].lexeme == ":") {
            Medico m;
            i += 2;
            if (i < n) { m.nombre = sinComillas(tokens[i].lexeme); i++; }

            while (i < n && tokens[i].lexeme != "[") i++;
            i++;

            while (i < n && tokens[i].lexeme != "]") {
                std::string attr = tokens[i].lexeme;
                if (attr == "especialidad" && i + 2 < n) {
                    m.especialidad = tokens[i + 2].lexeme;
                    i += 3;
                }
                else if (attr == "codigo" && i + 2 < n) {
                    m.codigo = sinComillas(tokens[i + 2].lexeme);
                    i += 3;
                }
                else {
                    i++;
                }
            }
            medicos.push_back(m);
        }

        // ── para citas
        else if (lex == "cita" && i + 1 < n && tokens[i + 1].lexeme == ":") {
            Cita c;
            i += 2;
            if (i < n) { c.nombrePaciente = sinComillas(tokens[i].lexeme); i++; }
            // saltar "con"
            while (i < n && tokens[i].lexeme != "con") i++;
            i++;
            if (i < n) { c.nombreMedico = sinComillas(tokens[i].lexeme); i++; }

            while (i < n && tokens[i].lexeme != "[") i++;
            i++;

            while (i < n && tokens[i].lexeme != "]") {
                std::string attr = tokens[i].lexeme;
                if (attr == "fecha" && i + 2 < n) {
                    c.fecha = tokens[i + 2].lexeme; i += 3;
                }
                else if (attr == "hora" && i + 2 < n) {
                    c.hora = tokens[i + 2].lexeme;  i += 3;
                }
                else {
                    i++;
                }
            }
            citas.push_back(c);
        }

		// ── para diagnostico ─────────────────────────────────────────
        else if (lex == "diagnostico" && i + 1 < n && tokens[i + 1].lexeme == ":") {
            Diagnostico d;
            i += 2;
            if (i < n) { d.nombrePaciente = sinComillas(tokens[i].lexeme); i++; }

            while (i < n && tokens[i].lexeme != "[") i++;
            i++;

            while (i < n && tokens[i].lexeme != "]") {
                std::string attr = tokens[i].lexeme;
                if (attr == "condicion" && i + 2 < n) {
                    d.condicion = sinComillas(tokens[i + 2].lexeme); i += 3;
                }
                else if (attr == "medicamento" && i + 2 < n) {
                    d.medicamento = sinComillas(tokens[i + 2].lexeme); i += 3;
                }
                else if (attr == "dosis" && i + 2 < n) {
                    d.dosis = tokens[i + 2].lexeme; i += 3;
                }
                else {
                    i++;
                }
            }
            diagnosticos.push_back(d);
        }

        else { i++; }
    }
}

void ReportGenerator::setTokens(const std::vector<Token>& tokens) {
    this->tokens = tokens;
};

void ReportGenerator::setErrors(const std::vector<ErrorToken>& errors) {
    this->errors = errors;
};



void ReportGenerator::cruzarDatos() {
    // Cruzce de dianosticos con pacientes
    for (auto& p : pacientes) {
        for (auto& d : diagnosticos) {
            if (d.nombrePaciente == p.nombre) {
                p.condicion = d.condicion;
                p.medicamento = d.medicamento;
                p.dosis = d.dosis;
                break;
            }
        }
    }

    // Cruce de citas con médicos
    for (auto& m : medicos) {
        std::set<std::string> pacientesUnicos;
        for (auto& c : citas) {
            if (c.nombreMedico == m.nombre) {
                m.totalCitas++;
                pacientesUnicos.insert(c.nombrePaciente);
                c.especialidadMedico = m.especialidad;
            }
        }
        m.totalPacientesDistintos = (int)pacientesUnicos.size();
    }

    // para conflictos en citas
    for (int i = 0; i < (int)citas.size(); i++) {
        for (int j = i + 1; j < (int)citas.size(); j++) {
            if (citas[i].nombreMedico == citas[j].nombreMedico &&
                citas[i].fecha == citas[j].fecha &&
                citas[i].hora == citas[j].hora) {
                citas[i].esConflicto = true;
                citas[j].esConflicto = true;
            }
        }
    }

    // citas en orden cronologico
    std::sort(citas.begin(), citas.end(), [](const Cita& a, const Cita& b) {
        if (a.fecha != b.fecha) return a.fecha < b.fecha;
        return a.hora < b.hora;
        });
}

std::string ReportGenerator::htmlHeader(const std::string& titulo, const std::string& extraCSS) {
    return R"(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<title>)" + titulo + R"(</title>
<style>
  body { font-family: Arial, sans-serif; margin: 0; padding: 24px;
         background: #f5f5f5; color: #333; }
  h1   { color: #1a4731; border-bottom: 3px solid #1a4731;
         padding-bottom: 8px; }
  h2   { color: #2e7d52; margin-top: 32px; }
  table{ width: 100%; border-collapse: collapse; background: #fff;
         border-radius: 8px; overflow: hidden;
         box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
  th   { background: #1a4731; color: #fff; padding: 12px 16px;
         text-align: left; font-size: 14px; }
  td   { padding: 10px 16px; border-bottom: 1px solid #eee;
         font-size: 14px; }
  tr:last-child td { border-bottom: none; }
  tr:hover td      { background: #f9f9f9; }
  .badge { padding: 4px 10px; border-radius: 12px; font-weight: bold;
           font-size: 12px; display: inline-block; }
  .verde   { background: #d4edda; color: #155724; }
  .naranja { background: #fff3cd; color: #856404; }
  .rojo    { background: #f8d7da; color: #721c24; }
  .azul    { background: #cce5ff; color: #004085; }
)" + extraCSS + R"(
</style>
</head>
<body>
<h1>MedLexer — )" + titulo + R"(</h1>
)";
}

void ReportGenerator::generateReporte1(const std::string& outputPath) {
    std::ostringstream html;
    html << htmlHeader("Historial de Pacientes");
    html << R"(
<table>
  <thead>
    <tr>
      <th>Paciente</th><th>Edad</th><th>Sangre</th>
      <th>Diagnóstico activo</th><th>Medicamento / Dosis</th><th>Estado</th>
    </tr>
  </thead>
  <tbody>
)";
    for (const auto& p : pacientes) {
        std::string estado, badgeClass;
        if (p.condicion.empty()) {
            estado = "SIN DIAG."; badgeClass = "naranja";
        }
        else {
            estado = "ACTIVO"; badgeClass = "verde";
        }

        std::string diagnosticoCell = p.condicion.empty()
            ? "<em>Sin diagnóstico registrado</em>" : p.condicion;
        std::string medCell = p.condicion.empty()
            ? "—" : p.medicamento + " / " + p.dosis;

        html << "<tr>"
            << "<td>" << p.nombre << "</td>"
            << "<td>" << p.edad << "</td>"
            << "<td>" << p.tipoSangre << "</td>"
            << "<td>" << diagnosticoCell << "</td>"
            << "<td>" << medCell << "</td>"
            << "<td><span class='badge " << badgeClass << "'>"
            << estado << "</span></td>"
            << "</tr>\n";
    }
    html << "</tbody></table>";
	html << "</body></html>";
    escribirArchivo(outputPath, html.str());
}

void ReportGenerator::generateReporte2(const std::string& outputPath) {
    std::ostringstream html;
    html << htmlHeader("Carga de Médicos por Especialidad", "");
    html << R"(
<table>
  <thead>
    <tr>
      <th>Médico</th><th>Código</th><th>Especialidad</th>
      <th>Citas prog.</th><th>Pacientes</th><th>Nivel de carga</th>
    </tr>
  </thead>
  <tbody>
)";
    for (const auto& m : medicos) {
        std::string nivel, badgeClass;
        int c = m.totalCitas;
        if (c <= 3) { nivel = "BAJA";     badgeClass = "azul"; }
        else if (c <= 6) { nivel = "NORMAL";   badgeClass = "verde"; }
        else if (c <= 8) { nivel = "ALTA";     badgeClass = "naranja"; }
        else { nivel = "SATURADA"; badgeClass = "rojo"; }

        html << "<tr>"
            << "<td>" << m.nombre << "</td>"
            << "<td>" << m.codigo << "</td>"
            << "<td>" << m.especialidad << "</td>"
            << "<td>" << m.totalCitas << "</td>"
            << "<td>" << m.totalPacientesDistintos << "</td>"
            << "<td><span class='badge " << badgeClass << "'>"
            << nivel << "</span></td>"
            << "</tr>\n";
    }
    html << "</tbody></table>";
	html << "</body></html>";
    escribirArchivo(outputPath, html.str());
}

// ── Reporte 3 — Agenda de Citas 

void ReportGenerator::generateReporte3(const std::string& outputPath) {
    std::ostringstream html;
    html << htmlHeader("Agenda de Citas", R"(
  .conflicto td { background: #f8d7da !important; }
  .pendiente td { background: #fff3cd !important; }
)");
    html << R"(
<table>
  <thead>
    <tr>
      <th>Fecha</th><th>Hora</th><th>Paciente</th>
      <th>Médico</th><th>Especialidad</th><th>Estado</th>
    </tr>
  </thead>
  <tbody>
)";
    // Fecha de hoy para detectar estado pendiente
    time_t t = time(nullptr);
    struct tm t_struct;
    localtime_s(&t_struct, &t); // Recibe el destino como primer argumento

    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &t_struct);
    std::string hoy(buf);

    for (const auto& c : citas) {
        std::string rowClass = "", estado = "", badgeClass = "";

        if (c.esConflicto) {
            rowClass = "conflicto";
            estado = "&#9888; CONFLICTO"; badgeClass = "rojo";
        }
        else if (c.fecha > hoy) {
            rowClass = "pendiente";
            estado = "PENDIENTE"; badgeClass = "naranja";
        }
        else {
            estado = "CONFIRMADA"; badgeClass = "verde";
        }

        html << "<tr class='" << rowClass << "'>"
            << "<td>" << c.fecha << "</td>"
            << "<td>" << c.hora << "</td>"
            << "<td>" << c.nombrePaciente << "</td>"
            << "<td>" << c.nombreMedico << "</td>"
            << "<td>" << c.especialidadMedico << "</td>"
            << "<td><span class='badge " << badgeClass << "'>"
            << estado << "</span></td>"
            << "</tr>\n";
    }
    html << "</tbody></table>";
	html << "</body></html>";
    escribirArchivo(outputPath, html.str());
}

// ── Reporte 4

void ReportGenerator::generateReporte4(const std::string& outputPath) {
    // Calcular KPIs
    int totalConflictos = 0;
    for (const auto& c : citas) if (c.esConflicto) totalConflictos++;
    totalConflictos /= 2; 

    int conDiagnostico = 0;
    double sumaEdades = 0;
    for (const auto& p : pacientes) {
        if (!p.condicion.empty()) conDiagnostico++;
        sumaEdades += p.edad;
    }
    double promedioEdad = pacientes.empty() ? 0
        : sumaEdades / pacientes.size();

    // Medicamento más prescrito
    std::map<std::string, int> frecMed;
    for (const auto& d : diagnosticos) frecMed[d.medicamento]++;
    std::string medMasPrescrito = "—";
    int maxFrecMed = 0;
    for (const auto& kv : frecMed) {
        if (kv.second > maxFrecMed) { maxFrecMed = kv.second; medMasPrescrito = kv.first; }
    }

    // Especialidad con mayor carga
    std::string espMaxCarga = "—", medicoMaxCarga = "—";
    int maxCitas = 0;
    for (const auto& m : medicos) {
        if (m.totalCitas > maxCitas) {
            maxCitas = m.totalCitas;
            espMaxCarga = m.especialidad;
            medicoMaxCarga = m.nombre;
        }
    }

    std::ostringstream html;
    html << htmlHeader("Estadístico General", R"(
  .kpi-grid { display: grid; grid-template-columns: repeat(2,1fr);
              gap: 16px; margin: 16px 0 32px; }
  .kpi-card { background: #fff; border-radius: 8px; padding: 16px 20px;
              box-shadow: 0 2px 8px rgba(0,0,0,0.08);
              border-left: 4px solid #1a4731; }
  .kpi-label { font-size: 13px; color: #666; margin-bottom: 4px; }
  .kpi-value { font-size: 22px; font-weight: bold; color: #1a4731; }
  .barra-bg  { background: #e0e0e0; border-radius: 4px;
               height: 14px; width: 100%; }
  .barra-fg  { background: #1a4731; border-radius: 4px; height: 14px; }
  .barra-rojo { background: #dc3545; }
)");

    // Sección A — KPIs
    html << "<h2>Sección A — Indicadores clave</h2>\n";
    html << "<div class='kpi-grid'>\n";

    auto kpi = [&](const std::string& label, const std::string& value) {
        html << "<div class='kpi-card'>"
            << "<div class='kpi-label'>" << label << "</div>"
            << "<div class='kpi-value'>" << value << "</div>"
            << "</div>\n";
        };

    kpi("Total de pacientes", std::to_string(pacientes.size()));
    kpi("Total de médicos", std::to_string(medicos.size()));
    kpi("Total de citas", std::to_string(citas.size()));
    kpi("Citas con conflicto", std::to_string(totalConflictos) + " ⚠");
    kpi("Pacientes con diagnóstico",
        std::to_string(conDiagnostico) + " de " +
        std::to_string(pacientes.size()) + " (" +
        std::to_string(pacientes.empty() ? 0 :
            (int)(conDiagnostico * 100 / pacientes.size())) + "%)");
    kpi("Medicamento más prescrito",
        medMasPrescrito + " (" + std::to_string(maxFrecMed) + " pacientes)");
    kpi("Especialidad con mayor carga",
        espMaxCarga + " — " + medicoMaxCarga);

    char bufEdad[16];
    snprintf(bufEdad, sizeof(bufEdad), "%.1f años", promedioEdad);
    kpi("Promedio de edad", bufEdad);
    html << "</div>\n";

    // Sección B — Distribución por especialidad
    html << "<h2>Sección B — Distribución de carga por especialidad</h2>\n";
    html << R"(
<table>
  <thead>
    <tr>
      <th>Especialidad</th><th>Médicos</th>
      <th>Citas</th><th>Pacientes</th><th>Ocupación</th>
    </tr>
  </thead>
  <tbody>
)";
    // Agrupar por especialidad
    std::map<std::string, std::pair<int, int>> espStats; // citas, medicos
    std::map<std::string, std::set<std::string>> espPacientes;
    for (const auto& m : medicos) espStats[m.especialidad].second++;
    for (const auto& c : citas) {
        espStats[c.especialidadMedico].first++;
        espPacientes[c.especialidadMedico].insert(c.nombrePaciente);
    }

    int totalCitasHosp = (int)citas.size();
    for (const auto& kv : espStats) {
        int pct = totalCitasHosp == 0 ? 0
            : (int)(kv.second.first * 100 / totalCitasHosp);
        bool saturada = pct > 80;
        std::string barraClass = saturada ? "barra-rojo" : "";

        html << "<tr>"
            << "<td><strong>" << kv.first << "</strong></td>"
            << "<td>" << kv.second.second << "</td>"
            << "<td>" << kv.second.first << "</td>"
            << "<td>" << (int)espPacientes[kv.first].size() << "</td>"
            << "<td style='width:200px'>"
            << "<div class='barra-bg'>"
            << "<div class='barra-fg " << barraClass << "' style='width:"
            << pct << "%'></div></div>"
            << " <small>" << pct << "%</small></td>"
            << "</tr>\n";
    }
    html << "</tbody></table>";
	html << "</body></html>";
    escribirArchivo(outputPath, html.str());
}

