#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <limits>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#else
#include <unistd.h>
#define CLEAR_SCREEN "clear"
#endif

using namespace std;

// Clase base Producto
template<typename T>
class Producto {
protected:
    string nombre;
    string codigoBarras;
    float precio;
    int cantidad;
    string categoria;
    T fechaVencimiento;

public:
    Producto(string nombre, string codigoBarras, float precio, int cantidad, string categoria, T fechaVencimiento)
        : nombre(nombre), codigoBarras(codigoBarras), precio(precio), cantidad(cantidad), categoria(categoria), fechaVencimiento(fechaVencimiento) {}

    virtual ~Producto() {}

    string getNombre() const { return nombre; }
    string getCodigoBarras() const { return codigoBarras; }
    float getPrecio() const { return precio; }
    int getCantidad() const { return cantidad; }
    string getCategoria() const { return categoria; }
    T getFechaVencimiento() const { return fechaVencimiento; }

    void setPrecio(float nuevoPrecio) { precio = nuevoPrecio; }
    void setCantidad(int nuevaCantidad) { cantidad = nuevaCantidad; }

    virtual void mostrarInformacion(ostream& os) const {
        os << "Nombre: " << nombre << endl;
        os << "Codigo de Barras: " << codigoBarras << endl;
        os << "Precio: $" << precio << endl;
        os << "Cantidad: " << cantidad << endl;
        os << "Categoria: " << categoria << endl;
        os << "Fecha de Vencimiento: " << fechaVencimiento << endl;
    }
};

// Clase Inventario para gestionar los productos
template<typename T>
class Inventario {
private:
    vector<Producto<T>*> productos;

public:
    ~Inventario() {
        for (Producto<T>* producto : productos) {
            delete producto;
        }
    }

    void agregarProducto(Producto<T>* producto) {
        productos.push_back(producto);
    }

    void actualizarProducto(string codigoBarras, float nuevoPrecio, int nuevaCantidad) {
        for (Producto<T>* producto : productos) {
            if (producto->getCodigoBarras() == codigoBarras) {
                producto->setPrecio(nuevoPrecio);
                producto->setCantidad(nuevaCantidad);
                break;
            }
        }
    }

    void eliminarProducto(string codigoBarras) {
        auto it = remove_if(productos.begin(), productos.end(), [&codigoBarras](Producto<T>* producto) {
            return producto->getCodigoBarras() == codigoBarras;
        });
        if (it != productos.end()) {
            delete *it;
            productos.erase(it, productos.end());
        }
    }

    Producto<T>* buscarProductoPorNombre(string nombre) {
        for (Producto<T>* producto : productos) {
            if (producto->getNombre() == nombre) {
                return producto;
            }
        }
        return nullptr;
    }

    Producto<T>* buscarProductoPorCodigoBarras(string codigoBarras) {
        for (Producto<T>* producto : productos) {
            if (producto->getCodigoBarras() == codigoBarras) {
                return producto;
            }
        }
        return nullptr;
    }

    vector<Producto<T>*> buscarProductoPorCategoria(string categoria) {
        vector<Producto<T>*> resultados;
        for (Producto<T>* producto : productos) {
            if (producto->getCategoria() == categoria) {
                resultados.push_back(producto);
            }
        }
        return resultados;
    }

    vector<Producto<T>*> obtenerProductosProximosAVencer() {
        vector<Producto<T>*> resultados;
        time_t ahora = time(0);
        for (Producto<T>* producto : productos) {
            if (producto->getFechaVencimiento() != 0 && difftime(producto->getFechaVencimiento(), ahora) < 30 * 24 * 60 * 60) { // 30 días
                resultados.push_back(producto);
            }
        }
        return resultados;
    }

    template<typename Func>
    void ordenarProductos(Func criterio) {
        sort(productos.begin(), productos.end(), criterio);
    }

    void generarInformePorCategoria(string categoria, const string& nombreArchivo) {
        ofstream archivo(nombreArchivo);
        if (!archivo) {
            cout << "Error al abrir el archivo" << endl;
            return;
        }

        vector<Producto<T>*> productosCategoria = buscarProductoPorCategoria(categoria);
        for (Producto<T>* producto : productosCategoria) {
            producto->mostrarInformacion(archivo);
            archivo << "-----------------------------------" << endl;
        }

        archivo.close();
        cout << "Informe generado en el archivo: " << nombreArchivo << endl;
    }

    void generarInformeProductosProximosAVencer(const string& nombreArchivo) {
        ofstream archivo(nombreArchivo);
        if (!archivo) {
            cout << "Error al abrir el archivo" << endl;
            return;
        }

        vector<Producto<T>*> proximosAVencer = obtenerProductosProximosAVencer();
        for (Producto<T>* producto : proximosAVencer) {
            producto->mostrarInformacion(archivo);
            archivo << "-----------------------------------" << endl;
        }

        archivo.close();
        cout << "Informe generado en el archivo: " << nombreArchivo << endl;
    }

    void generarInformeCompleto(const string& nombreArchivo) {
        ofstream archivo(nombreArchivo);
        if (!archivo) {
            cout << "Error al abrir el archivo" << endl;
            return;
        }

        for (Producto<T>* producto : productos) {
            producto->mostrarInformacion(archivo);
            archivo << "-----------------------------------" << endl;
        }

        archivo.close();
        cout << "Informe generado en el archivo: " << nombreArchivo << endl;
    }
};

// Función para mostrar el menú
void mostrarMenu() {
    cout << R"(
=============================
  Sistema de Gestion de Inventario
=============================
1. Agregar producto
2. Actualizar producto
3. Eliminar producto
4. Buscar producto
5. Generar informes
6. Salir
Seleccione una opcion: )";
}

// Función para validar la entrada de un número entero
int validarEntero() {
    int numero;
    while (!(cin >> numero)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada no valida. Por favor, ingrese un numero entero: ";
    }
    return numero;
}

// Función para validar la entrada de un número flotante
float validarFlotante() {
    float numero;
    while (!(cin >> numero)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada no valida. Por favor, ingrese un numero flotante: ";
    }
    return numero;
}

// Función para pausar y limpiar la pantalla
void pausaYLimpiar() {
    cout << "Presione cualquier tecla para continuar..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    system(CLEAR_SCREEN);
}

// Función para agregar un nuevo producto
template<typename T>
void agregarProducto(Inventario<T>& inventario) {
    string nombre, codigoBarras, categoria;
    float precio;
    int cantidad;
    T fechaVencimiento = 0;

    cout << "Ingrese el nombre del producto: ";
    cin.ignore();
    getline(cin, nombre);
    cout << "Ingrese el codigo de barras: ";
    getline(cin, codigoBarras);
    cout << "Ingrese el precio: ";
    precio = validarFlotante();
    cout << "Ingrese la cantidad: ";
    cantidad = validarEntero();
    cout << "Ingrese la categoria (Electronica, Ropa, Alimentos): ";
    cin.ignore();
    getline(cin, categoria);

    transform(categoria.begin(), categoria.end(), categoria.begin(), ::tolower); // Convertir a minúsculas

    if (categoria == "electronica") {
        inventario.agregarProducto(new Producto<T>(nombre, codigoBarras, precio, cantidad, "Electronica", fechaVencimiento));
    } else if (categoria == "ropa") {
        inventario.agregarProducto(new Producto<T>(nombre, codigoBarras, precio, cantidad, "Ropa", fechaVencimiento));
    } else if (categoria == "alimentos") {
        int dia, mes, ano;
        cout << "Ingrese la fecha de vencimiento (dia mes ano): ";
        dia = validarEntero();
        mes = validarEntero();
        ano = validarEntero();
        tm fecha = {0, 0, 0, dia, mes - 1, ano - 1900};
        fechaVencimiento = mktime(&fecha);
        inventario.agregarProducto(new Producto<T>(nombre, codigoBarras, precio, cantidad, "Alimentos", fechaVencimiento));
    } else {
        cout << "Categoria no valida!" << endl;
        return;
    }

    cout << "Producto agregado exitosamente!" << endl;
    pausaYLimpiar();
}

// Función para actualizar un producto
template<typename T>
void actualizarProducto(Inventario<T>& inventario) {
    string codigoBarras;
    float nuevoPrecio;
    int nuevaCantidad;

    cout << "Ingrese el codigo de barras del producto a actualizar: ";
    cin.ignore();
    getline(cin, codigoBarras);
    cout << "Ingrese el nuevo precio: ";
    nuevoPrecio = validarFlotante();
    cout << "Ingrese la nueva cantidad: ";
    nuevaCantidad = validarEntero();

    inventario.actualizarProducto(codigoBarras, nuevoPrecio, nuevaCantidad);
    cout << "Producto actualizado exitosamente!" << endl;
    pausaYLimpiar();
}

// Función para eliminar un producto
template<typename T>
void eliminarProducto(Inventario<T>& inventario) {
    string codigoBarras;

    cout << "Ingrese el codigo de barras del producto a eliminar: ";
    cin.ignore();
    getline(cin, codigoBarras);

    inventario.eliminarProducto(codigoBarras);
    cout << "Producto eliminado exitosamente!" << endl;
    pausaYLimpiar();
}

// Función para buscar un producto
template<typename T>
void buscarProducto(Inventario<T>& inventario) {
    int opcion;
    string entrada;

    cout << "Buscar por: \n1. Nombre \n2. Codigo de Barras \n3. Categoria \n---->: ";
    opcion = validarEntero();

    Producto<T>* producto;
    vector<Producto<T>*> productos;

    switch (opcion) {
        case 1:
            cout << "Ingrese el nombre del producto: ";
            cin.ignore();
            getline(cin, entrada);
            producto = inventario.buscarProductoPorNombre(entrada);
            if (producto) {
                producto->mostrarInformacion(cout);
            } else {
                cout << "Producto no encontrado" << endl;
            }
            break;
        case 2:
            cout << "Ingrese el codigo de barras: ";
            cin.ignore();
            getline(cin, entrada);
            producto = inventario.buscarProductoPorCodigoBarras(entrada);
            if (producto) {
                producto->mostrarInformacion(cout);
            } else {
                cout << "Producto no encontrado" << endl;
            }
            break;
        case 3:
            cout << "Ingrese la categoria: ";
            cin.ignore();
            getline(cin, entrada);
            transform(entrada.begin(), entrada.end(), entrada.begin(), ::tolower); // Convertir a minúsculas
            productos = inventario.buscarProductoPorCategoria(entrada);
            if (!productos.empty()) {
                for (Producto<T>* prod : productos) {
                    prod->mostrarInformacion(cout);
                    cout << "-----------------------------------" << endl;
                }
            } else {
                cout << "No se encontraron productos en esta categoria" << endl;
            }
            break;
        default:
            cout << "Opcion no valida!" << endl;
            break;
    }
    pausaYLimpiar();
}

// Función para generar informes
template<typename T>
void generarInformes(Inventario<T>& inventario) {
    int opcion;
    string categoria;

    cout << "Generar informe: \n1. Por Categoria \n2. Productos Proximos a Vencer \n3. Completo  \n----: ";
    opcion = validarEntero();

    switch (opcion) {
        case 1:
            cout << "Ingrese la categoria: ";
            cin.ignore();
            getline(cin, categoria);
            transform(categoria.begin(), categoria.end(), categoria.begin(), ::tolower); // Convertir a minúsculas
            inventario.generarInformePorCategoria(categoria, "informe_categoria.txt");
            break;
        case 2:
            inventario.generarInformeProductosProximosAVencer("informe_proximos_vencer.txt");
            break;
        case 3:
            inventario.generarInformeCompleto("informe_completo.txt");
            break;
        default:
            cout << "Opcion no valida!" << endl;
            break;
    }
    pausaYLimpiar();
}

int main() {
    Inventario<time_t> inventario; // Utiliza time_t como tipo de fecha de vencimiento

    int opcion;
    while (true) {
        mostrarMenu();
        opcion = validarEntero();

        switch (opcion) {
            case 1:
                agregarProducto(inventario);
                break;
            case 2:
                actualizarProducto(inventario);
                break;
            case 3:
                eliminarProducto(inventario);
                break;
            case 4:
                buscarProducto(inventario);
                break;
            case 5:
                generarInformes(inventario);
                break;
            case 6:
                cout << "Saliendo del sistema..." << endl;
                return 0;
            default:
                cout << "Opcion no valida! Por favor intente de nuevo." << endl;
                break;
        }
    }

    return 0;
}
