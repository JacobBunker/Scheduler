#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

int getLineCount(char* fn) {
	FILE *fileptr;
    int count_lines = 0;
    fileptr = fopen(fn, "r");
    char chr = getc(fileptr);
    while (chr != EOF)
    {
        //Count whenever new line is encountered
        if (chr == '\n')
        {
            count_lines = count_lines + 1;
        }
        //take next character from file.
        chr = getc(fileptr);
    }
    fclose(fileptr); //close file.
    printf("There are %d lines in %s\n", count_lines, fn);
    return count_lines;
}

void loadVocabulary(char* file1, char* file2, char** nouns, char** adjectives) {
	printf("loading %s and %s\n", file1, file2);
	int i = 0;
	FILE *fp;
	fp = fopen(file1, "r");
	char buff[32];
	while(fscanf(fp, "%32s", buff) != EOF) {
		nouns[i] = (char *) malloc(sizeof(buff));
		memcpy(nouns[i], buff, sizeof(buff));
		//printf("noun: %s\n", &nouns[i]);
		i++;
	}
	fclose(fp);
	fp = fopen(file2, "r");
	i = 0;
	while(fscanf(fp, "%32s", buff) != EOF) {
		adjectives[i] = (char *) malloc(sizeof(buff));
		memcpy(adjectives[i], buff, sizeof(buff));
		//printf("adjectives: %s\n", adjectives[i]);
		i++;
	}
	fclose(fp);
	printf("loaded vocabulary\n");
}

void performOnePointCrossover(int target, unsigned char** genome_buffer, int genome_size, int population_count) {
	int cross_partner = target;
	while(cross_partner == target) { cross_partner = rand() % population_count; }
	int cross_point = (rand() % (genome_size - 1)) + 1; //assumes genome size greater than two lmao
	unsigned char * cross_buffer = (unsigned char *) malloc(cross_point*sizeof(char));
	memcpy(cross_buffer, genome_buffer[target], cross_point*sizeof(char));
	memcpy(genome_buffer[target], genome_buffer[cross_partner], cross_point*sizeof(char));
	memcpy(genome_buffer[cross_partner], cross_buffer, cross_point*sizeof(char));
	free(cross_buffer);
}

void performBitMutation(int target, unsigned char** genome_buffer, int genome_size) { 
	int point = rand() % genome_size;
	unsigned char bit = 1 << (rand() % 8);
	genome_buffer[target][point] = genome_buffer[target][point] ^ bit;
}

struct Location
{
	char name[64];
	double coordinates[2];
};

struct Cargo
{
	char name[64];
	float size;
	int pickup_location;
	int dropoff_location;
	int pickup_deadline;
	int dropoff_deadline;
	char picked_up;
	char delivered;
	int contract;
};

struct Ship
{
	double coordinates[2];
	int destination;
	double distance_to_destination;
	double time_to_destination;
	int cargo[256];
	int cargo_amount;
	int cargo_capacity;
	float speed;
	float operating_cost;
	float fuel_range;
};

struct Contract
{
	int cargo[16];
	int cargo_count;
	double reward;
	double top_bid;
	double late_penalty;
	int top_bidder;
};

struct Scheduler
{
	char name[64];
	double cash;
	int ships[256];
	int ship_count;
	int outstanding_bids[256];
	int bid_count;
};


void printScheduler(struct Scheduler t) {
	printf("Scheduler %s:\n", t.name);
	printf("cash: %f, ship_count: %d, bid_count: %d \n", t.cash, t.ship_count, t.bid_count);
	int i = 0;
	while(i < t.ship_count) {
		printf("ship %d: %d\n", i, t.ships[i]);
		i++;
	}
	i = 0;
	while(i < t.bid_count) {
		printf("bid %d: %d\n", i, t.outstanding_bids[i]);
		i++;
	}
}

void printContract(struct Contract t, int i) {
	printf("Contract %d is for $%.2f and involves %d cargos\n", i, t.top_bid, t.cargo_count);
	printf("starting bid: $%.2f, top bidder: %d\n", t.reward, t.top_bidder);
}

int generateContracts(char* cfile, struct Contract* contracts, int contract_count, struct Cargo* cargos, char** nouns, 
					char** adjectives, int noun_count, int adjective_count, struct Location* locations, int location_count) {
	printf("Generating %d Contracts...\n", contract_count);
	FILE *fp;
	fp = fopen(cfile, "w+");
	int cargo_count = 0;
	int i = 0;
	while(i < contract_count) {
		struct Contract t;
		int c_amt = (rand() % 4) + 1;
		fprintf(fp, "Contract %d has %d items:\n", i, c_amt);
		int z = 0;
		while(z < c_amt) {
			struct Cargo c;
			c.size = ((rand() % 100) + 1)*0.1;
			c.pickup_location = rand() % location_count;
			c.dropoff_location = rand() % location_count;
			while(c.dropoff_location == c.pickup_location) {
				c.dropoff_location = rand() % location_count;
			}
			c.pickup_deadline = rand() % 1000;
			c.dropoff_deadline = c.pickup_deadline + (rand() % 1000);
			c.delivered = 0; c.picked_up = 0;
			c.contract = i;
			t.cargo[z] = cargo_count;
			int adj_r = rand() % adjective_count;
			int noun_r = rand() % noun_count;
			//printf("%d\n", c.pickup_deadline);
			strcpy(c.name, adjectives[adj_r]);
			strcat(c.name, " ");
			strcat(c.name, nouns[noun_r]);
			memcpy(&cargos[cargo_count], &c, sizeof(struct Cargo));
			fprintf(fp, "    '%s' (size: %.2f) shipping from %s by %d hours to %s by %d hours\n",
				 	cargos[cargo_count].name, c.size, locations[cargos[cargo_count].pickup_location].name, cargos[cargo_count].pickup_deadline,
				  	locations[cargos[cargo_count].dropoff_location].name, cargos[cargo_count].dropoff_deadline);
			cargo_count++;
			z++;
		}
		t.reward = c_amt * ((rand() % 10000) + 100.0);
		t.cargo_count = c_amt;
		t.top_bid = t.reward;
		t.top_bidder = -1;
		t.late_penalty = 0.0;//(t.reward / c_amt) * (rand() % 20) * 0.01; 
		fprintf(fp, "    reward: %0.2f\n", t.reward);
		memcpy(&contracts[i], &t, sizeof(struct Contract));
		i++;
	}
	fclose(fp);
	printf("wrote contract details to %s\n", cfile);
	return cargo_count;
}

int main(int argc, const char* argv[])
{
	printf("\nScheduler Program\n\n");
	printf("sizeof char: %ld\n", sizeof(char));
	srand(time(0));
	int i;

	struct Location locations[3];
	int location_count = 3;
	strcpy(locations[0].name, "Port Isoclous");
	strcpy(locations[1].name, "Port Chappero");
	strcpy(locations[2].name, "Port Melody");
	locations[0].coordinates[0] = 0.0;
	locations[0].coordinates[1] = 0.0;
	locations[1].coordinates[0] = 2.0;
	locations[1].coordinates[1] = 0.0;
	locations[2].coordinates[0] = 0.0;
	locations[2].coordinates[1] = 5.0;

	char file1[] = "nouns.txt";
	char file2[] = "adjectives.txt";
	char file3[] = "contracts.txt";

	int noun_count = getLineCount(file1);
	int adjective_count = getLineCount(file2);

	char** nouns = malloc(noun_count * sizeof(char *));
	char** adjectives = malloc(adjective_count * sizeof(char *));

	loadVocabulary(file1, file2, nouns, adjectives);

	int contract_count = 12;
	struct Cargo* cargos = (struct Cargo*) malloc(contract_count*16*sizeof(struct Cargo));
	struct Contract* contracts = (struct Contract*) malloc(contract_count*sizeof(struct Contract));
	int cargo_count = generateContracts(file3, contracts, contract_count, cargos, nouns, adjectives, noun_count, adjective_count, locations, location_count);
	printf("%d cargo generated\n", cargo_count);

	float crossover_probability = 0.05;
	float mutation_probability = 0.001;
	int population_count = 100000;
	int generation_count = 1000;
	int genome_size = 50*32;

	unsigned char** genomes = malloc(population_count * sizeof(char *));
	i = 0; 
	while(i < population_count) {
		genomes[i] = (unsigned char *) malloc(genome_size*sizeof(char));
		int x = 0;
		while(x < genome_size) {
			genomes[i][x] = (unsigned char) random();
			x++;
		}
		i++;
	}

	unsigned char** genome_buffer = malloc(population_count * sizeof(char *));
	i = 0; 
	while(i < population_count) {
		genome_buffer[i] = (unsigned char *) malloc(genome_size*sizeof(char));
		i++;
	}

	int simulation_length = 2000;
	int log_level = 2;

	double *fitnesses = (double *)malloc(sizeof(double)*population_count);

	printf("beginning simulation...\n");

	int generation = 0;
	while(generation < generation_count) {
		//STEP 1: evaluate the fitness of the population
		if(log_level > 0) {printf("generation %d/%d\n", generation, generation_count);}
		int current_target = 0;
		while(current_target < population_count) {
			if(log_level > 2) {printf("evaluating subject %d\n", current_target);}
			int wait = 0;
			int genome_step = 0;
			int hour = 0;
			unsigned target_ship, target_location;
			unsigned short target_cargo;
			double new_x, new_y;
			double travel_time;

			struct Scheduler adam = {.cash=0.0, .ships[0]=0, .ship_count=1, .bid_count=0};
			strcpy(adam.name, "Adam");

			struct Ship ships[1];
			int ship_count = 1;
			ships[0].coordinates[0] = 0.0;
			ships[0].coordinates[1] = 0.0;
			ships[0].destination = 0;
			ships[0].distance_to_destination = 1.0;
			ships[0].time_to_destination = 0;
			ships[0].cargo_amount = 0;
			ships[0].cargo_capacity = 1000;
			ships[0].speed = 0.5;
			ships[0].operating_cost = 0.0;
			ships[0].fuel_range = 100.0;
			int cargo_picked_up = 0;
			int cargo_dropped_off = 0;
			int late_penalties = 0;
			int check_complete = 0;

			i = 0; while(i < cargo_count) {cargos[i].delivered = 0; cargos[i].picked_up = 0; i++;}

			while(hour < simulation_length) {
				//printf("hour %d\n", hour);
				//printf("wait: %d\n", wait);
				if(wait == 0 && genome_step < genome_size) {
					//process genome
					switch((unsigned int) (genomes[current_target][genome_step] >> 6)) {
						case 0: //wait
							wait += (unsigned int) genomes[current_target][genome_step + 1];
							if(log_level > 2) {printf("wait %d\n", wait);}
							break;
						case 1: //pickup cargo]
							target_ship = ((unsigned int) genomes[current_target][genome_step + 1]) % adam.ship_count;
							if(ships[adam.ships[target_ship]].time_to_destination == 0.0) {
								target_cargo = ((unsigned short) genomes[current_target][genome_step + 2]) << 8;
								target_cargo = (target_cargo | (unsigned short) genomes[current_target][genome_step + 3]) % cargo_count;
								if(cargos[target_cargo].pickup_location == ships[adam.ships[target_ship]].destination) {
									if(cargos[target_cargo].picked_up == 0) { //cargo is present
										if(cargos[target_cargo].size + ships[adam.ships[target_ship]].cargo_amount <= ships[adam.ships[target_ship]].cargo_capacity) {
											if(cargos[target_cargo].pickup_deadline >= hour) { //on-time pickup
												if(log_level > 2) {printf("pickup cargo %d with ship %d success\n", target_cargo, target_ship);}
											} else { //late pickup, incur penalty
												adam.cash -= contracts[cargos[target_cargo].contract].late_penalty;
												late_penalties++;
												if(log_level > 2) {printf("pickup cargo %d with ship %d success but LATE!\n", target_cargo, target_ship);}
											}
											cargos[target_cargo].picked_up = 1;
											ships[adam.ships[target_ship]].cargo_amount += cargos[target_cargo].size;
											cargo_picked_up++;
										} else { //cargo cannot fit on the ship
											if(log_level > 3) {printf("pickup cargo %d with ship %d FAIL: cargo cannot fit on vessel\n", target_cargo, target_ship);}
										}
									} else { //cargo was already picked up
										if(log_level > 3) {printf("pickup cargo %d with ship %d FAIL: cargo already picked up\n", target_cargo, target_ship);}
									}
								} else if(log_level > 3) {printf("pickup cargo %d with ship %d FAIL: ship at wrong port\n", target_cargo, target_ship);}
							} else if(log_level > 3) {printf("pickup cargo %d with ship %d FAIL: ship not docked\n", target_cargo, target_ship);}
							break;
						case 2: //dropoff cargo
							target_ship = ((unsigned int) genomes[current_target][genome_step + 1]) % adam.ship_count;
							if(ships[adam.ships[target_ship]].time_to_destination == 0.0) {
								target_cargo = ((unsigned short) genomes[current_target][genome_step + 2]) << 8;
								target_cargo = (target_cargo | (unsigned short) genomes[current_target][genome_step + 3]) % cargo_count;
								if(cargos[target_cargo].dropoff_location == ships[adam.ships[target_ship]].destination) {
									if(cargos[target_cargo].picked_up == 1) { //did we pick up the cargo?
										if(cargos[target_cargo].delivered == 0) { //is cargo alread delivered?
											if(cargos[target_cargo].dropoff_deadline >= hour) {
												if(log_level > 2) {printf("dropoff cargo %d with ship %d success\n", target_cargo, target_ship);}
											} else {
												adam.cash -= contracts[cargos[target_cargo].contract].late_penalty;
												late_penalties++;
												if(log_level > 2) {printf("dropoff cargo %d with ship %d success but LATE!\n", target_cargo, target_ship);}
											}
											cargos[target_cargo].delivered = 1;
											//check if contract is complete
											check_complete = 1; i = 0; 
											while(i < contracts[cargos[target_cargo].contract].cargo_count) { 
												int temp = contracts[cargos[target_cargo].contract].cargo[i];
												if(cargos[temp].delivered == 0) {
													check_complete = 0;
												}
												i++;
											}
											if(check_complete == 1) { //contract is completed
												adam.cash += contracts[cargos[target_cargo].contract].reward;
												if(log_level > 2) {printf("CONTRACT %d COMPLETED!\n", cargos[target_cargo].contract);}
											}
											ships[adam.ships[target_ship]].cargo_amount -= cargos[target_cargo].size;
											cargo_dropped_off++;
										} else if(log_level > 3) {printf("dropoff cargo %d with ship %d FAIL: cargo was already delivered\n", target_cargo, target_ship);}
									} else if(log_level > 3) {printf("dropoff cargo %d with ship %d FAIL: cargo was not picked up\n", target_cargo, target_ship);}
								} else if(log_level > 3) {printf("dropoff cargo %d with ship %d FAIL: ship at wrong port\n", target_cargo, target_ship);}
							} else if(log_level > 3) {printf("dropoff cargo %d with ship %d FAIL: ship not docked\n", target_cargo, target_ship);}
							break;
						case 3: //direct ship
							target_ship = ((unsigned int) genomes[current_target][genome_step + 1]) % adam.ship_count;
							target_location = ((unsigned int) genomes[current_target][genome_step + 2]) % location_count;
							//we only have the coordinates of the ship at the last time it was directed and the time to the next destination
							//A: calculate the current coordinates of the ship based upon current time to destination
							new_x = locations[ships[adam.ships[target_ship]].destination].coordinates[0] - ships[adam.ships[target_ship]].coordinates[0];
							new_y = locations[ships[adam.ships[target_ship]].destination].coordinates[1] - ships[adam.ships[target_ship]].coordinates[1];
							travel_time = ships[adam.ships[target_ship]].distance_to_destination / ships[adam.ships[target_ship]].speed;
							if(travel_time > 0.0) { //make sure we aren't already arrived at the destination to avoid NaN error
								ships[adam.ships[target_ship]].coordinates[0] = new_x*(ships[adam.ships[target_ship]].time_to_destination / travel_time);
								ships[adam.ships[target_ship]].coordinates[1] = new_y*(ships[adam.ships[target_ship]].time_to_destination / travel_time);
							}
							else {
								ships[adam.ships[target_ship]].coordinates[0] = locations[ships[adam.ships[target_ship]].destination].coordinates[0];
								ships[adam.ships[target_ship]].coordinates[1] = locations[ships[adam.ships[target_ship]].destination].coordinates[1];
							}
							//coordinates are now updated appropriately
							//we must now update the time and distance to the new destination from the present coordinates
							ships[adam.ships[target_ship]].distance_to_destination = hypot(locations[target_location].coordinates[0] - ships[adam.ships[target_ship]].coordinates[0],
											 												locations[target_location].coordinates[1] - ships[adam.ships[target_ship]].coordinates[1]);
							ships[adam.ships[target_ship]].time_to_destination = ships[adam.ships[target_ship]].distance_to_destination / ships[adam.ships[target_ship]].speed;
							ships[adam.ships[target_ship]].destination = target_location;
							
							if(log_level > 3) {printf("direct ship %d to %s, distance: %f, time: %f\n", adam.ships[target_ship], locations[target_location].name, 
														ships[adam.ships[target_ship]].distance_to_destination, ships[adam.ships[target_ship]].time_to_destination);}
							break;
						default:
							//how did it get here?
							printf("genome type switch reached default?!");
					} 
					genome_step += 4;
				} else {
					//run an hour of simulation
					i = 0;
					while(i < ship_count) {
						if(ships[i].time_to_destination > 0.0) {
							ships[i].time_to_destination -= 1;
							if(ships[i].time_to_destination <= 0.0) {
								if(log_level > 2) {printf("ship %d has reached %s\n", i, locations[ships[i].destination].name); ships[i].time_to_destination = 0.0;}
							}
						}
						i++;
					}
					wait--;
				}

				hour++;
			}
			//dole out participation points
			adam.cash += cargo_picked_up;
			adam.cash += cargo_dropped_off*10;
			adam.cash -= late_penalties;

			fitnesses[current_target] = adam.cash;
			if(log_level > 2) {printf("genome %d achieved fitness %f\n", current_target, fitnesses[current_target]);}
			if(log_level > 2) {printf("\tcargo picked up: %d\n\tcargo dropped off: %d \n\tlate penalties: %d\n", cargo_picked_up, cargo_dropped_off, late_penalties);}
			current_target++;
		}
		//STEP 2: construct the roulette
		//normalize the fitnesses
		double sum_val = 0.0;
		i = 0;
		while(i < population_count) {
			sum_val += fitnesses[i];
			i++;
		}
		if(log_level > 1) {printf("sum: %f\taverage: %f\n", sum_val, (sum_val/population_count));}
		double* roulette = (double *) malloc(sizeof(double)*population_count); 
		roulette[0] = (fitnesses[0] / sum_val);
		i = 1;
		while(i < population_count) {
			if(sum_val != 0) {
				roulette[i] = roulette[i-1] + (fitnesses[i] / sum_val);
				if(log_level > 3) {printf("%d\t%f\t%f\n", i, (fitnesses[i]/sum_val), roulette[i]);}
			} else {
				roulette[i] = 0.0;
			}
			i++;
		}

		int n = 0;
		i = 0;
		while(i < population_count) {
			double r = rand();
			r = r / RAND_MAX;
			n = 0;
			while(n < population_count) {
				if(roulette[n] > r) {
					break;
				}
				n++;
			}
			if(n != 0) {
				n = n - 1;
			}
			if(log_level > 3) {printf("%d\t%f\tgenome %d chosen\n", i, r, n);}
			memcpy(genome_buffer[i], genomes[n], genome_size*sizeof(char));
			i++;
		}

		//do crossover
		int crossovers = 0;
		if(population_count > 1) {
			i = 0; 
			while(i < population_count) {
				double r = rand(); r = r / RAND_MAX;
				if(r <= crossover_probability) {
					performOnePointCrossover(i, genome_buffer, genome_size, population_count);
					crossovers++;
				} 
				i++;
			}
		}

		//do mutation
		int mutations = 0;
		i = 0; 
		while(i < population_count) {
			double r = rand(); r = r / RAND_MAX;
			if(r <= mutation_probability) {
				performBitMutation(i, genome_buffer, genome_size);
				mutations++;
			} 
			i++;
		}
		if(log_level > 1) {printf("crossovers: %d\tmutations: %d\n", crossovers, mutations);}
		i = 0; while(i < population_count) {memcpy(genomes[i], genome_buffer[i], genome_size*sizeof(char)); i++;}

		generation++;
	}

	i = 0; while(i < population_count) {free(genomes[i]); i++;}
	i = 0; while(i < noun_count) {free(nouns[i]); i++;}
	i = 0; while(i < adjective_count) {free(adjectives[i]); i++;}
	free(contracts);
	free(cargos);
	//free(&adjectives);
	//free(&nouns);
}
