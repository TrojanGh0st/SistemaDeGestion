#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>

using namespace std;

// Clase Libro
class Libro {
public:
    string titulo;
    string autor;
    string ISBN;
    string genero;
    int anio;
    bool disponible;

    Libro(string titulo, string autor, string ISBN, string genero, int anio, bool disponible)
        : titulo(titulo), autor(autor), ISBN(ISBN), genero(genero), anio(anio), disponible(disponible) {}
};

// Clase Usuario
class Usuario {
public:
    string nombre;
    int ID;
    string tipo; // estudiante, profesor, investigador

    Usuario(string nombre, int ID, string tipo)
        : nombre(nombre), ID(ID), tipo(tipo) {}
};

// Clase Prestamo
class Prestamo {
public:
    int usuarioID;
    string ISBN;
    time_t fechaPrestamo;
    time_t fechaDevolucion;

    Prestamo(int usuarioID, string ISBN, time_t fechaPrestamo, time_t fechaDevolucion = 0)
        : usuarioID(usuarioID), ISBN(ISBN), fechaPrestamo(fechaPrestamo), fechaDevolucion(fechaDevolucion) {}
};

// Clase Biblioteca
class Biblioteca {
private:
    vector<Libro> catalogo;
    vector<Usuario> usuarios;
    vector<Prestamo> prestamos;

public:
    void agregarLibro(Libro libro) {
        catalogo.push_back(libro);
    }

    void modificarLibro(string ISBN, string nuevoTitulo, string nuevoAutor, string nuevoGenero, int nuevoAnio, bool nuevaDisponibilidad) {
        auto it = find_if(catalogo.begin(), catalogo.end(), [&ISBN](const Libro& libro) {
            return libro.ISBN == ISBN;
        });
        if (it != catalogo.end()) {
            it->titulo = nuevoTitulo;
            it->autor = nuevoAutor;
            it->genero = nuevoGenero;
            it->anio = nuevoAnio;
            it->disponible = nuevaDisponibilidad;
        } else {
            throw runtime_error("Libro no encontrado");
        }
    }

    void eliminarLibro(string ISBN) {
        auto it = remove_if(catalogo.begin(), catalogo.end(), [&ISBN](const Libro& libro) {
            return libro.ISBN == ISBN;
        });
        if (it != catalogo.end()) {
            catalogo.erase(it, catalogo.end());
        } else {
            throw runtime_error("Libro no encontrado");
        }
    }

    void registrarUsuario(Usuario usuario) {
        usuarios.push_back(usuario);
    }

    void realizarPrestamo(int usuarioID, string ISBN) {
        auto it = find_if(catalogo.begin(), catalogo.end(), [&ISBN](const Libro& libro) {
            return libro.ISBN == ISBN;
        });
        if (it != catalogo.end()) {
            if (it->disponible) {
                it->disponible = false;
                time_t now = time(0);
                prestamos.push_back(Prestamo(usuarioID, ISBN, now));
            } else {
                throw runtime_error("Libro no disponible");
            }
        } else {
            throw runtime_error("Libro no encontrado");
        }
    }

    void realizarDevolucion(int usuarioID, string ISBN) {
        auto it = find_if(prestamos.begin(), prestamos.end(), [&](const Prestamo& prestamo) {
            return prestamo.usuarioID == usuarioID && prestamo.ISBN == ISBN && prestamo.fechaDevolucion == 0;
        });
        if (it != prestamos.end()) {
            it->fechaDevolucion = time(0);
            auto libroIt = find_if(catalogo.begin(), catalogo.end(), [&ISBN](const Libro& libro) {
                return libro.ISBN == ISBN;
            });
            if (libroIt != catalogo.end()) {
                libroIt->disponible = true;
            } else {
                throw runtime_error("Libro no encontrado en el catálogo al devolver");
            }
        } else {
            throw runtime_error("Préstamo no encontrado");
        }
    }

    void generarInformeLibrosPopulares() {
        // Aquí puedes implementar la lógica para generar informes de libros más populares
        // Por ahora, simplemente mostraré un mensaje para indicar que se ha llamado a esta función
        cout << "Generando informe de libros populares..." << endl;
    }

    void generarInformeUsuariosActivos() {
        // Aquí puedes implementar la lógica para generar informes de usuarios más activos
        // Por ahora, simplemente mostraré un mensaje para indicar que se ha llamado a esta función
        cout << "Generando informe de usuarios activos..." << endl;
    }

    vector<Libro> buscarLibros(string criterio, string valor) {
        vector<Libro> resultados;
        transform(valor.begin(), valor.end(), valor.begin(), ::tolower); // Convertir valor a minúsculas para comparación sin distinción de mayúsculas
        for (const auto& libro : catalogo) {
            string valorLibro;
            if (criterio == "titulo") {
                valorLibro = libro.titulo;
            } else if (criterio == "autor") {
                valorLibro = libro.autor;
            } else if (criterio == "isbn") {
                valorLibro = libro.ISBN;
            } else if (criterio == "genero") {
                valorLibro = libro.genero;
            }

            transform(valorLibro.begin(), valorLibro.end(), valorLibro.begin(), ::tolower); // Convertir valorLibro a minúsculas para comparación sin distinción de mayúsculas

            if (valorLibro.find(valor) != string::npos) { // Buscar valor dentro de valorLibro
                resultados.push_back(libro);
            }
        }
        return resultados;
    }

    Libro verDetallesLibro(string ISBN) {
        auto it = find_if(catalogo.begin(), catalogo.end(), [&ISBN](const Libro& libro) {
            return libro.ISBN == ISBN;
        });
        if (it != catalogo.end()) {
            return *it;
        } else {
            throw runtime_error("Libro no encontrado");
        }
    }

    vector<Prestamo> verHistorialPrestamos(int usuarioID) {
        vector<Prestamo> historial;
        for (const auto& prestamo : prestamos) {
            if (prestamo.usuarioID == usuarioID) {
                historial.push_back(prestamo);
            }
        }
        return historial;
    }

    vector<Prestamo> verHistorialCompleto() {
        return prestamos;
    }

    // Funciones para guardar y cargar datos desde archivos
    void guardarDatos() {
        ofstream archivoLibros("libros.txt");
        for (const auto& libro : catalogo) {
            archivoLibros << libro.titulo << ";" << libro.autor << ";" << libro.ISBN << ";" << libro.genero << ";"
                          << libro.anio << ";" << libro.disponible << endl;
        }
        archivoLibros.close();

        ofstream archivoUsuarios("usuarios.txt");
        for (const auto& usuario : usuarios) {
            archivoUsuarios << usuario.nombre << ";" << usuario.ID << ";" << usuario.tipo << endl;
        }
        archivoUsuarios.close();

        ofstream archivoPrestamos("prestamos.txt");
        for (const auto& prestamo : prestamos) {
            archivoPrestamos << prestamo.usuarioID << ";" << prestamo.ISBN << ";" << prestamo.fechaPrestamo << ";"
                             << prestamo.fechaDevolucion << endl;
        }
        archivoPrestamos.close();
    }

    void cargarDatos() {
        ifstream archivoLibros("libros.txt");
        string linea;
        while (getline(archivoLibros, linea)) {
            stringstream ss(linea);
            string titulo, autor, ISBN, genero;
            int anio;
            bool disponible;
            getline(ss, titulo, ';');
            getline(ss, autor, ';');
            getline(ss, ISBN, ';');
            getline(ss, genero, ';');
            ss >> anio;
            ss.ignore();  // Ignorar el delimitador ';'
            ss >> disponible;
            catalogo.push_back(Libro(titulo, autor, ISBN, genero, anio, disponible));
        }
        archivoLibros.close();

        ifstream archivoUsuarios("usuarios.txt");
        while (getline(archivoUsuarios, linea)) {
            stringstream ss(linea);
            string nombre, tipo;
            int ID;
            getline(ss, nombre, ';');
            ss >> ID;
            ss.ignore();  // Ignorar el delimitador ';'
            getline(ss, tipo, ';');
            usuarios.push_back(Usuario(nombre, ID, tipo));
        }
        archivoUsuarios.close();

        ifstream archivoPrestamos("prestamos.txt");
        while (getline(archivoPrestamos, linea)) {
            stringstream ss(linea);
            int usuarioID;
            string ISBN;
            time_t fechaPrestamo, fechaDevolucion;
            ss >> usuarioID;
            ss.ignore();  // Ignorar el delimitador ';'
            getline(ss, ISBN, ';');
            ss >> fechaPrestamo;
            ss.ignore();  // Ignorar el delimitador ';'
            ss >> fechaDevolucion;
            prestamos.push_back(Prestamo(usuarioID, ISBN, fechaPrestamo, fechaDevolucion));
        }
        archivoPrestamos.close();
    }
};

// Función para mostrar el menú
void mostrarMenu() {
    cout << "### Biblioteca Central - Sistema de Gestión ###" << endl;
    cout << "1. Agregar libro" << endl;
    cout << "2. Modificar libro" << endl;
    cout << "3. Eliminar libro" << endl;
    cout << "4. Registrar usuario" << endl;
    cout << "5. Realizar préstamo" << endl;
    cout << "6. Realizar devolución" << endl;
    cout << "7. Generar informe de libros populares" << endl;
    cout << "8. Generar informe de usuarios activos" << endl;
    cout << "9. Buscar libros" << endl;
    cout << "10. Ver detalles de un libro" << endl;
    cout << "11. Ver historial de préstamos de un usuario" << endl;
    cout << "12. Ver historial completo de préstamos" << endl;
    cout << "13. Guardar datos" << endl;
    cout << "14. Cargar datos" << endl;
    cout << "15. Salir" << endl;
}

int main() {
    Biblioteca biblioteca;
    biblioteca.cargarDatos(); // Cargar datos al inicio del programa

    int opcion;
    do {
        mostrarMenu();
        cout << "Ingrese una opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                string titulo, autor, ISBN, genero;
                int anio;
                bool disponible;
                cout << "Ingrese título: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, titulo);
                cout << "Ingrese autor: ";
                getline(cin, autor);
                cout << "Ingrese ISBN: ";
                getline(cin, ISBN);
                cout << "Ingrese género: ";
                getline(cin, genero);
                cout << "Ingrese año: ";
                cin >> anio;
                cout << "¿Está disponible? (1: sí, 0: no): ";
                cin >> disponible;
                biblioteca.agregarLibro(Libro(titulo, autor, ISBN, genero, anio, disponible));
                break;
            }
            case 2: {
                string ISBN, nuevoTitulo, nuevoAutor, nuevoGenero;
                int nuevoAnio;
                bool nuevaDisponibilidad;
                cout << "Ingrese ISBN del libro a modificar: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, ISBN);
                cout << "Ingrese nuevo título: ";
                getline(cin, nuevoTitulo);
                cout << "Ingrese nuevo autor: ";
                getline(cin, nuevoAutor);
                cout << "Ingrese nuevo género: ";
                getline(cin, nuevoGenero);
                cout << "Ingrese nuevo año: ";
                cin >> nuevoAnio;
                cout << "¿Está disponible? (1: sí, 0: no): ";
                cin >> nuevaDisponibilidad;
                biblioteca.modificarLibro(ISBN, nuevoTitulo, nuevoAutor, nuevoGenero, nuevoAnio, nuevaDisponibilidad);
                break;
            }
            case 3: {
                string ISBN;
                cout << "Ingrese ISBN del libro a eliminar: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, ISBN);
                biblioteca.eliminarLibro(ISBN);
                break;
            }
            case 4: {
                string nombre, tipo;
                int ID;
                cout << "Ingrese nombre: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, nombre);
                cout << "Ingrese ID: ";
                cin >> ID;
                cout << "Ingrese tipo (estudiante, profesor, investigador): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, tipo);
                biblioteca.registrarUsuario(Usuario(nombre, ID, tipo));
                break;
            }
            case 5: {
                int usuarioID;
                string ISBN;
                cout << "Ingrese ID del usuario: ";
                cin >> usuarioID;
                cout << "Ingrese ISBN del libro: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, ISBN);
                biblioteca.realizarPrestamo(usuarioID, ISBN);
                break;
            }
            case 6: {
                int usuarioID;
                string ISBN;
                cout << "Ingrese ID del usuario: ";
                cin >> usuarioID;
                cout << "Ingrese ISBN del libro: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, ISBN);
                biblioteca.realizarDevolucion(usuarioID, ISBN);
                break;
            }
            case 7: {
                biblioteca.generarInformeLibrosPopulares();
                break;
            }
            case 8: {
                biblioteca.generarInformeUsuariosActivos();
                break;
            }
            case 9: {
                string criterio, valor;
                cout << "Ingrese criterio de búsqueda (titulo, autor, isbn, genero): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, criterio);
                cout << "Ingrese valor: ";
                getline(cin, valor);
                vector<Libro> resultados = biblioteca.buscarLibros(criterio, valor);
                cout << "Resultados de búsqueda:" << endl;
                for (const auto& libro : resultados) {
                    cout << "Título: " << libro.titulo << ", Autor: " << libro.autor << ", ISBN: " << libro.ISBN
                         << ", Género: " << libro.genero << ", Año: " << libro.anio << ", Disponible: " << (libro.disponible ? "Sí" : "No") << endl;
                }
                break;
            }
            case 10: {
                string ISBN;
                cout << "Ingrese ISBN del libro: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, ISBN);
                try {
                    Libro libro = biblioteca.verDetallesLibro(ISBN);
                    cout << "Detalles del libro:" << endl;
                    cout << "Título: " << libro.titulo << ", Autor: " << libro.autor << ", ISBN: " << libro.ISBN
                         << ", Género: " << libro.genero << ", Año: " << libro.anio << ", Disponible: " << (libro.disponible ? "Sí" : "No") << endl;
                } catch (const runtime_error& e) {
                    cout << e.what() << endl;
                }
                break;
            }
            case 11: {
                int usuarioID;
                cout << "Ingrese ID del usuario: ";
                cin >> usuarioID;
                vector<Prestamo> historial = biblioteca.verHistorialPrestamos(usuarioID);
                cout << "Historial de préstamos:" << endl;
                for (const auto& prestamo : historial) {
                    cout << "ISBN: " << prestamo.ISBN << ", Fecha de préstamo: " << ctime(&prestamo.fechaPrestamo)
                         << ", Fecha de devolución: " << (prestamo.fechaDevolucion != 0 ? ctime(&prestamo.fechaDevolucion) : "No devuelto") << endl;
                }
                break;
            }
            case 12: {
                vector<Prestamo> historialCompleto = biblioteca.verHistorialCompleto();
                cout << "Historial completo de préstamos:" << endl;
                for (const auto& prestamo : historialCompleto) {
                    cout << "Usuario ID: " << prestamo.usuarioID << ", ISBN: " << prestamo.ISBN << ", Fecha de préstamo: " << ctime(&prestamo.fechaPrestamo)
                         << ", Fecha de devolución: " << (prestamo.fechaDevolucion != 0 ? ctime(&prestamo.fechaDevolucion) : "No devuelto") << endl;
                }
                break;
            }
            case 13: {
                biblioteca.guardarDatos();
                cout << "Datos guardados con éxito." << endl;
                break;
            }
            case 14: {
                biblioteca.cargarDatos();
                cout << "Datos cargados con éxito." << endl;
                break;
            }
            case 15: {
                cout << "Saliendo del programa..." << endl;
                break;
            }
            default:
                cout << "Opción no válida, intente de nuevo." << endl;
                break;
        }

    } while (opcion != 15);

    return 0;
}
