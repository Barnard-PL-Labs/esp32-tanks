import pygame
import sys

# Initialize Pygame
pygame.init()

# Set up the display
width, height = 800, 600
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption('Rover Simulator')

# Colors
WHITE = (255, 255, 255)
GRAY = (200, 200, 200)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
YELLOW = (255, 255, 0)

# Define the grid size and cell size
grid_size = 10
cell_size = min(width, height) // grid_size

# Create the grid
grid = [[0] * grid_size for _ in range(grid_size)]

class Goal:
    def __init__(self, x, y):
        self.x = x
        self.y = y

goal = Goal(5, 5)

class Obstacle:
    def __init__(self, gate_x, gate_y, sensor_x, sensor_y):
        self.gate_x = gate_x
        self.gate_y = gate_y
        self.sensor_x = sensor_x
        self.sensor_y = sensor_y
        self.active = True

    def check_if_headlight_points_to(self, rover):
        x_diff = self.sensor_x - rover.x
        y_diff = self.sensor_y - rover.y

        if rover.headlights_on and (abs(x_diff) + abs(y_diff)) == 1:
            if rover.direction == 'N' and y_diff < 0:
                return True
            elif rover.direction == 'E' and x_diff > 0:
                return True
            elif rover.direction == 'S' and y_diff > 0:
                return True
            elif rover.direction == 'W' and x_diff < 0:
                return True

        return False

# Create obstacles
obstacle_positions = [(1, 1, 1, 2), (3, 3, 3, 4)]  # Format: (gate_x, gate_y, sensor_x, sensor_y)
obstacles = [Obstacle(gate_x, gate_y, sensor_x, sensor_y) for gate_x, gate_y, sensor_x, sensor_y in obstacle_positions]


# Rover class
class Rover:
    def __init__(self, x, y, direction):
        self.x = x
        self.y = y
        self.direction = direction
        self.headlights_on = False

    def can_move_to(self, x, y):
        for obstacle in obstacles:
            if obstacle.gate_x == x and obstacle.gate_y == y and obstacle.active:
                return False
            if obstacle.sensor_x == x and obstacle.sensor_y == y:
                return False
        return True

    def move_forward(self):
        new_x, new_y = self.x, self.y
        if self.direction == 'N':
            new_y -= 1
        elif self.direction == 'E':
            new_x += 1
        elif self.direction == 'S':
            new_y += 1
        elif self.direction == 'W':
            new_x -= 1

        if self.can_move_to(new_x, new_y):
            self.x, self.y = new_x, new_y

    def turn_left(self):
        directions = ['N', 'W', 'S', 'E']
        current_index = directions.index(self.direction)
        self.direction = directions[(current_index - 1) % 4]

    def turn_right(self):
        directions = ['N', 'E', 'S', 'W']
        current_index = directions.index(self.direction)
        self.direction = directions[(current_index + 1) % 4]

    def toggle_headlights(self):
        self.headlights_on = not self.headlights_on

    def draw_headlights(self):
        if self.headlights_on:
            if self.direction == 'N':
                pygame.draw.polygon(screen, YELLOW, [(self.x * cell_size, self.y * cell_size - cell_size), ((self.x + 1) * cell_size, self.y * cell_size - cell_size), (self.x * cell_size + cell_size // 2, self.y * cell_size - cell_size // 2)])
            elif self.direction == 'E':
                pygame.draw.polygon(screen, YELLOW, [((self.x + 1) * cell_size + cell_size, self.y * cell_size), ((self.x + 1) * cell_size + cell_size, (self.y + 1) * cell_size), ((self.x + 1) * cell_size + cell_size // 2, self.y * cell_size + cell_size // 2)])
            elif self.direction == 'S':
                pygame.draw.polygon(screen, YELLOW, [(self.x * cell_size, (self.y + 1) * cell_size + cell_size), ((self.x + 1) * cell_size, (self.y + 1) * cell_size + cell_size), (self.x * cell_size + cell_size // 2, (self.y + 1) * cell_size + cell_size // 2)])
            elif self.direction == 'W':
                pygame.draw.polygon(screen, YELLOW, [(self.x * cell_size - cell_size, self.y * cell_size), (self.x * cell_size - cell_size, (self.y + 1) * cell_size), (self.x * cell_size - cell_size // 2, self.y * cell_size + cell_size // 2)])

# Create obstacles
obstacle_positions = [(1, 1, 1, 2), (3, 3, 3, 4)]  # Format: (gate_x, gate_y, sensor_x, sensor_y)
obstacles = [Obstacle(gate_x, gate_y, sensor_x, sensor_y) for gate_x, gate_y, sensor_x, sensor_y in obstacle_positions]
deactivated_obstacles = []

# Create Rovers  
rovers = [Rover(0, 0, 'N'), Rover(9, 9, 'S')]  # Two rovers at opposite corners of the grid
current_rover_index = 0  # Index of the currently controlled rover

def draw_grid():
    for y in range(grid_size):
        for x in range(grid_size):
            rect = pygame.Rect(x * cell_size, y * cell_size, cell_size, cell_size)
            gate_colors = {True: BLACK, False: GRAY}  # Use GRAY color for deactivated gates
            for obstacle in obstacles:
                if obstacle.gate_x == x and obstacle.gate_y == y:
                    if obstacle.active:
                        pygame.draw.rect(screen, BLACK, rect)
                    else:
                        pygame.draw.rect(screen, BLACK, rect, 1)
                if obstacle.sensor_x == x and obstacle.sensor_y == y:
                    pygame.draw.rect(screen, RED, rect)
            # Draw the goal tile
            if x == goal.x and y == goal.y:
                pygame.draw.polygon(screen, YELLOW, [(rect.centerx, rect.top), (rect.right, rect.centery), (rect.centerx, rect.bottom), (rect.left, rect.centery)])
                pygame.draw.polygon(screen, BLACK, [(rect.centerx, rect.top), (rect.right, rect.centery), (rect.centerx, rect.bottom), (rect.left, rect.centery)], 1)
                

# Main loop
while True:
    screen.fill(WHITE)

    draw_grid()    
        
    # Draw rover
    for idx, rover in enumerate(rovers):
        rover_rect = pygame.Rect(rover.x * cell_size, rover.y * cell_size, cell_size, cell_size)
        fill_color = GREEN
        outline_color = RED if idx == current_rover_index else None
        outline_thickness = 3
        
        # Define the points for each direction
        direction_points = {
            'N': [(rover_rect.centerx, rover_rect.top), (rover_rect.right, rover_rect.bottom), (rover_rect.left, rover_rect.bottom)],
            'E': [(rover_rect.right, rover_rect.centery), (rover_rect.left, rover_rect.top), (rover_rect.left, rover_rect.bottom)],
            'S': [(rover_rect.centerx, rover_rect.bottom), (rover_rect.left, rover_rect.top), (rover_rect.right, rover_rect.top)],
            'W': [(rover_rect.left, rover_rect.centery), (rover_rect.right, rover_rect.top), (rover_rect.right, rover_rect.bottom)]
        }
        
        points = direction_points[rover.direction]
        pygame.draw.polygon(screen, fill_color, points)
        if outline_color:
            pygame.draw.polygon(screen, outline_color, points, outline_thickness)

            # Draw headlights
            rover.draw_headlights()
            
            # Check light sensors for each rover
                    # Check light sensors for each rover
            for rover in rovers:
                for o in obstacles:
                    if any(o.check_if_headlight_points_to(r) for r in rovers):
                        o.active = False
                    else:
                        o.active = True

    # Check if any rover is on the goal tile
    for rover in rovers:
        if rover.x == goal.x and rover.y == goal.y:
            font = pygame.font.Font(None, 36)
            text = font.render('You win!', True, GREEN)
            text_rect = text.get_rect(center=(width // 2, height // 2))
            screen.blit(text, text_rect)
            break  # Stop checking if one rover reaches the goal

    pygame.display.flip()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                # Turn the current rover left
                rovers[current_rover_index].turn_left()
            elif event.key == pygame.K_RIGHT:
                # Turn the current rover right
                rovers[current_rover_index].turn_right()
            elif event.key == pygame.K_UP:
                # Move the current rover forward
                rovers[current_rover_index].move_forward()
            elif event.key == pygame.K_SPACE:
                # Toggle headlights for the current rover
                rovers[current_rover_index].toggle_headlights()
            elif event.key == pygame.K_s:
                # Switch control to the other rover
                current_rover_index = (current_rover_index + 1) % len(rovers)

