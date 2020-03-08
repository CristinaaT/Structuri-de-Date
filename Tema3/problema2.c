#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

//Structura care va stoca o persoana (si vecinii ei)
typedef struct person_t {
	char name[6];
	int n_neighbours, capacity; //Folosite pentru vectorul de mai jos
	int *neighbours; //Vector cu id-urile vecinilor
} person_t;

//Returneaza index-ul persoanei cu numele dat
int find_person(person_t *v, int n, char *name)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (strcmp(v[i].name, name) == 0) 
			return i;
	}
	return -1;
}

//Adauga o persoana intr-un vector de persoane
//Redimensioneaza vectorul daca este necesar
//Returneaza pointer-ul catre vector (util in cazul redimensionarilor)
//Capacitatea se va modifica prin efect lateral
person_t *add_person(person_t *v, int n, int *p_capacity, char *name)
{
	//Verifica daca trebuie redimensionat vectorul
	if (n == *p_capacity) {
		*p_capacity *= 2;
		person_t *tmp = malloc(*p_capacity * sizeof(person_t));
		memcpy(tmp, v, n * sizeof(person_t));
		free(v);
		v = tmp;
	}

	//Adaugam noua persoana
	strcpy(v[n].name, name);
	v[n].capacity = 1;
	v[n].n_neighbours = 0;
	v[n].neighbours = malloc(sizeof(person_t *));

	return v;
}

//Adauga un vecin unui persoane
void add_neighbour(person_t *p_person, int neighbour_id)
{
	//Verifica daca trebuie redimensionat vectorul
	if (p_person->n_neighbours == p_person->capacity) {
		p_person->capacity *= 2;
		int *tmp = malloc(p_person->capacity * sizeof(int));
		memcpy(tmp, p_person->neighbours,
		       p_person->n_neighbours * sizeof(int));
		free(p_person->neighbours);
		p_person->neighbours = tmp;
	}

	//Adauga id-ul persoanei in vector
	p_person->neighbours[p_person->n_neighbours++] = neighbour_id;
}

//Marcheaza persoanele vizitate
//Returneaza numarul de persoane vizitate
int dfs(person_t *persons, int x, int *visited)
{
	int i, ans = 1; //Numarul de persoane vizitate
	visited[x] = 1;
	for (i = 0; i < persons[x].n_neighbours; i++) 
	{
		if (visited[persons[x].neighbours[i]]) 
			continue;
		ans += dfs(persons, persons[x].neighbours[i], visited);
	}

	return ans;
}

//Compara doua numere intregi
int less_than(const void *x, const void *y)
{
	int a = *((int *)x);
	int b = *((int *)y);
	return a - b;
}

int main(int argc, char **argv)
{
	if (argc != 8) {
		fprintf(stderr, "Prea multe sau prea putine argumente!\n");
		return -1;
	}

	//Buffere pentru citires
	char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];

	//Stocarea persoanelor
	int n_persons = 0, sum, capacity = 1, tax;
	person_t *persons = malloc(sizeof(person_t));

	//Stocarea dimensiunilor familiilor
	int n_families = 0, capacity1 = 1;
	int *family_sizes = malloc(sizeof(int));

	//Deschide fisierele de intrare/iesire
	FILE *input_file = fopen(argv[1], "rt");
	FILE *output_file = fopen(argv[2], "wt");
	if (input_file == NULL || output_file == NULL) 
	{
		fprintf(stderr, "Fisierele nu au putut fi deschise!\n");
		return -1;
	}

	//Citeste datele
	fscanf(input_file, "%d", &sum);
	int x, y;
	while (fscanf(input_file, "%s %s", buffer1, buffer2) == 2) 
	{
		x = find_person(persons, n_persons, buffer1);
		if (x == -1) 
		{
			//Prima data cand apare aceasta persoana
			persons = add_person(persons, n_persons, &capacity, buffer1);
			x = n_persons++;
		}

		y = find_person(persons, n_persons, buffer2);
		if (y == -1) 
		{
			//Prima data cand apare aceasta persoana
			persons = add_person(persons, n_persons, &capacity, buffer2);
			y = n_persons++;
		}

		//Adauga o muchie intre cele doua persoane
		//Graful este neorientat
		add_neighbour(&persons[x], y);
		add_neighbour(&persons[y], x);
	}

	//Vector pentru DFS
	int i, *visited = malloc(n_persons * sizeof(int));
	for (i = 0; i < n_persons; i++) 
		visited[i] = 0;

	//Gaseste numarul de membri din fiecare familie
	int ans;
	for (i = 0; i < n_persons; i++) 
	{
		if (visited[i]) 
			continue;

		ans = dfs(persons, i, visited);
		if (n_families == capacity1) 
		{
			//Redimensionam vectorul
			capacity1 *= 2;
			int *tmp = malloc(capacity1 * sizeof(int));
			memcpy(tmp, family_sizes, n_families * sizeof(int));
			free(family_sizes);
			family_sizes = tmp;
		}

		//Am gasit o noua dimensiune a unei familii
		family_sizes[n_families++] = ans;
	}

	//Sortam vectorul (necesar pentru a afisa in ordine)
	qsort(family_sizes, n_families, sizeof(int), less_than);

	//Afiseaza raspunsurile
	tax = sum / n_persons;
	fprintf(output_file, "%d\n%d\n%d\n", n_persons, tax, n_families);
	fprintf(output_file, "%d", family_sizes[0] * tax);
	for (i = 1; i < n_families; i++)
		fprintf(output_file, " %d", family_sizes[i] * tax);

	//Eliberea memoria
	for (i = 0; i < n_persons; i++) 
		free(persons[i].neighbours);
	free(persons);
	free(visited);
	free(family_sizes);

	//Inchide fisierele
	fclose(input_file); fclose(output_file);

	return 0;
}
