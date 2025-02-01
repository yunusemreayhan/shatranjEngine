import tkinter as tk
import chess

# Board settings
BOARD_SIZE = 600      # pixels
SQUARE_SIZE = BOARD_SIZE // 8

# Colors for the board squares
LIGHT_COLOR = "#F0D9B5"
DARK_COLOR = "#B58863"
# for file in ./icons/*.svg; do inkscape "$file" --export-filename="${file%.svg}.png" --export-width=$(inkscape "$file" --query-width | awk '{print int($1 * 0.65)}') --export-height=$(inkscape "$file" --query-height | awk '{print int($1 * 0.65)}'); done
# for file in ./icons/*.svg; do inkscape "$file" --export-filename="${file%.svg}.png" --export-width=$(inkscape "$file" --query-width | awk '{print int($1 * 0.85)}') --export-height=$(inkscape "$file" --query-height | awk '{print int($1 * 0.85)}'); done

class ChessGUI:
    def __init__(self, master):
        self.master = master
        master.title("Python Chess GUI with Custom Icons")

        # Create a Tkinter Canvas widget
        self.canvas = tk.Canvas(master, width=BOARD_SIZE, height=BOARD_SIZE)
        self.canvas.pack()

        # Initialize the chess board using python-chess
        self.board = chess.Board()

        # To track the selected square (if any)
        self.selected_square = None

        # Load custom piece images
        self.load_images()

        # Draw the board and pieces
        self.draw_board()
        self.draw_pieces()

        # Bind the click event
        self.canvas.bind("<Button-1>", self.on_click)

    def load_images(self):
        """
        Load your own custom icons from the "icons" directory.
        The keys are the piece symbols from python‑chess.
        White pieces are represented by uppercase letters, black by lowercase.
        """
        self.piece_images = {}
        pieces = ['P', 'R', 'N', 'B', 'Q', 'K']
        for p in pieces:
            try:
                # Load white piece image (e.g., "icons/wP.png")
                self.piece_images[p] = tk.PhotoImage(file=f"icons/w{p}.png")
                # Load black piece image (e.g., "icons/bP.png")
                self.piece_images[p.lower()] = tk.PhotoImage(file=f"icons/b{p}.png")
            except tk.TclError as e:
                print(f"Error loading image for piece {p}: {e}")
                raise

        # Keep a reference so images are not garbage-collected.
        self.image_refs = list(self.piece_images.values())

    def draw_board(self):
        """Draw the chessboard squares."""
        self.squares = {}
        self.canvas.delete("square")
        for rank in range(8):
            for file in range(8):
                x1 = file * SQUARE_SIZE
                y1 = (7 - rank) * SQUARE_SIZE  # Flip rank for correct orientation
                x2 = x1 + SQUARE_SIZE
                y2 = y1 + SQUARE_SIZE

                # Determine color based on file and rank
                color = LIGHT_COLOR if (file + rank) % 2 == 0 else DARK_COLOR

                square_id = self.canvas.create_rectangle(
                    x1, y1, x2, y2, fill=color, tags="square"
                )
                self.squares[(file, rank)] = square_id

    def draw_pieces(self):
        """Draw the chess pieces on the board using your custom icons."""
        # Remove any existing pieces
        self.canvas.delete("piece")
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is not None:
                # Convert python‑chess square to file and rank (0-indexed)
                file = chess.square_file(square)
                rank = chess.square_rank(square)
                x = file * SQUARE_SIZE + SQUARE_SIZE // 2
                y = (7 - rank) * SQUARE_SIZE + SQUARE_SIZE // 2
                # Retrieve the corresponding image from the dictionary.
                image = self.piece_images[piece.symbol()]
                # Place the image on the canvas.
                self.canvas.create_image(
                    x, y, image=image, tags="piece"
                )

    def highlight_square(self, file, rank):
        """Highlight the given square."""
        x1 = file * SQUARE_SIZE
        y1 = (7 - rank) * SQUARE_SIZE
        x2 = x1 + SQUARE_SIZE
        y2 = y1 + SQUARE_SIZE
        return self.canvas.create_rectangle(
            x1, y1, x2, y2, outline="blue", width=3, tags="highlight"
        )

    def on_click(self, event):
        """Handle click events on the canvas."""
        # Determine which square was clicked
        file = event.x // SQUARE_SIZE
        rank = 7 - (event.y // SQUARE_SIZE)
        clicked_square = chess.square(file, rank)

        # Clear any previous highlight
        self.canvas.delete("highlight")

        if self.selected_square is None:
            # Select a piece if one exists on the clicked square and if it is the side to move's piece
            piece = self.board.piece_at(clicked_square)
            if piece and ((piece.symbol().isupper() and self.board.turn) or
                          (piece.symbol().islower() and not self.board.turn)):
                self.selected_square = clicked_square
                self.highlight_square(file, rank)
        else:
            # Try to make a move from the selected square to the clicked square
            move = chess.Move(self.selected_square, clicked_square)
            if move in self.board.legal_moves:
                self.board.push(move)
                self.draw_board()
                self.draw_pieces()
            else:
                print("Illegal move attempted:", move)
            # Clear selection after move attempt
            self.selected_square = None


def main():
    root = tk.Tk()
    gui = ChessGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
