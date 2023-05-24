#include <stdio.h>
#include <SDL.h>

const int GRID_WIDTH = 50;
const int GRID_HEIGHT = 50;
const int CELL_SIZE = 10;
// Cell class
class Cell
{
public:
	Cell()
		: m_alive(false)
	{}

	Cell(int x, int y)
		: m_x(x), m_y(y), m_alive(false)
	{}

	void SetAlive(bool alive)
	{
		m_alive = alive;
	}

	bool IsAlive() const
	{
		return m_alive;
	}

	void ToggleState()
	{
		m_alive = !m_alive;
	}

private:
	int m_x;
	int m_y;
	bool m_alive;
};

// Grid class
class Grid
{
public:
	Grid()
	{
		// Initialize cells
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			for (int y = 0; y < GRID_HEIGHT; ++y)
			{
				m_cells[x][y] = Cell(x, y);
			}
		}
	}

	void ToggleCellState(int x, int y)
	{
		m_cells[x][y].ToggleState();
	}

	bool IsCellAlive(int x, int y) const
	{
		return m_cells[x][y].IsAlive();
	}
	void SetCellState(int x, int y, bool alive)
	{
		// Ensure that the given cell coordinates are within the grid boundaries
		if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT)
		{
			m_cells[x][y].SetAlive(alive);
		}
	}

	int CountAliveNeighbors(int x, int y)
	{
		int count = 0;

		// Check the 8 neighboring cells
		for (int i = -1; i <= 1; ++i)
		{
			for (int j = -1; j <= 1; ++j)
			{
				// Exclude the current cell itself
				if (i == 0 && j == 0)
					continue;

				// Calculate the coordinates of the neighboring cell
				int neighborX = x + i;
				int neighborY = y + j;

				// Check if the neighboring cell is within the grid boundaries and alive
				if (neighborX >= 0 && neighborX < GRID_WIDTH && neighborY >= 0 && neighborY < GRID_HEIGHT)
				{
					if (m_cells[neighborX][neighborY].IsAlive())
					{
						++count;
					}
				}
			}
		}

		return count;
	}
	void Update()
	{
		// Apply game rules and update cell states
		Grid updatedGrid = *this; // Create a copy of the current grid

		// Iterate over each cell in the grid
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			for (int y = 0; y < GRID_HEIGHT; ++y)
			{
				// Count the number of alive neighbors
				int aliveNeighbors = CountAliveNeighbors(x, y);

				// Apply the rules of Conway's Game of Life
				if (IsCellAlive(x, y))
				{
					// Cell is alive
					if (aliveNeighbors < 2 || aliveNeighbors > 3)
					{
						// Cell dies due to underpopulation or overpopulation
						updatedGrid.SetCellState(x, y, false);
					}
				}
				else
				{
					// Cell is dead
					if (aliveNeighbors == 3)
					{
						// Cell becomes alive due to reproduction
						updatedGrid.SetCellState(x, y, true);
					}
				}
			}
		}

		*this = updatedGrid; // Replace the original grid with the updated grid
	}
private:
	Cell m_cells[GRID_WIDTH][GRID_HEIGHT];
};
void DrawGrid(SDL_Renderer* renderer, Grid grid) {
	// Draw the grid
	for (int x = 0; x < GRID_WIDTH; ++x)
	{
		for (int y = 0; y < GRID_HEIGHT; ++y)
		{
			if (grid.IsCellAlive(x, y))
			{
				// Set color for live cells
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			else
			{
				// Set color for dead cells
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			}

			// Draw the cell
			SDL_Rect cellRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
			SDL_RenderFillRect(renderer, &cellRect);
		}
	}
}
void DrawGridBorder(SDL_Renderer* renderer, int width) {
	// Draw grid border
	int borderWidth = width; // Specify the desired border width
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

	// Draw multiple rectangles to simulate a wider border
	for (int i = 0; i < borderWidth; ++i)
	{
		// Calculate the dimensions of the border rectangle
		SDL_Rect borderRect = {
			-i, -i,
			(GRID_WIDTH * CELL_SIZE) + (i * 2),
			(GRID_HEIGHT * CELL_SIZE) + (i * 2)
		};
		SDL_RenderDrawRect(renderer, &borderRect);
	}
}
int main(int argc, char* args[])
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// Handle initialization error
		SDL_Log("SDL initialization failed: %s", SDL_GetError());
		return 1;
	}

	// Create a window
	SDL_Window* window = SDL_CreateWindow("My SDL Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	if (!window)
	{
		// Handle window creation error
		SDL_Log("Window creation failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Create a renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		SDL_Log("Renderer creation failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Create a grid
	Grid grid;

	// Main game loop
	bool running = true;
	int iterationCount = 0;
	while (running)
	{
		// Handle events
		SDL_Event event;
		int mouseX = 0;
		int mouseY = 0;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				// Check the specific key that was pressed
				switch (event.key.keysym.sym)
				{
				case SDLK_SPACE:
					// Space bar was pressed
					// Perform the desired action (e.g., toggle simulation state)
					grid.Update();
					iterationCount++;
					break;
				
				case SDLK_ESCAPE:
					// Escape key was pressed
					// Perform the desired action (e.g., exit the program)
					running = false;
					break;
					// Add more cases for other keys if needed
				}
				break;
			case SDL_QUIT:
				// Handle quit event (e.g., window close button)
				running = false;
				break;
				// Add more event types if needed
			case SDL_MOUSEBUTTONDOWN:
				// Get mouse coordinates
				int mouseX = event.button.x;
				int mouseY = event.button.y;

				// Check if the click is within the grid area
				if (mouseX >= 0 && mouseX < GRID_WIDTH * CELL_SIZE && mouseY >= 0 && mouseY < GRID_HEIGHT * CELL_SIZE)
				{
					// Convert mouse coordinates to cell coordinates
					int cellX = mouseX / CELL_SIZE;
					int cellY = mouseY / CELL_SIZE;

					// Toggle cell state
					grid.ToggleCellState(cellX, cellY);
				}
				break;
			}
		}

		// Clear the renderer
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Draw Grid
		DrawGrid(renderer, grid);
		// Draw Border
		DrawGridBorder(renderer, 10);

		// Update the screen
		SDL_RenderPresent(renderer);

		// Update the grid
		grid.Update();
	}

	// Clean up and exit
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}


