import tkinter as tk
import chess
import io
import os

from PIL import Image, ImageTk
import cairosvg

# Board settings
BOARD_SIZE = 600      # pixels
SQUARE_SIZE = BOARD_SIZE // 8

# Colors for the board squares
LIGHT_COLOR = "#F0D9B5"
DARK_COLOR = "#B58863"


class ChessGUI:
    def __init__(self, master):
        self.master = master
        master.title("Python Chess GUI with Editor Mode")

        # Create main frame to hold canvas and control panel.
        self.main_frame = tk.Frame(master)
        self.main_frame.pack()

        # Create a Tkinter Canvas widget for the board.
        self.canvas = tk.Canvas(self.main_frame, width=BOARD_SIZE, height=BOARD_SIZE)
        self.canvas.grid(row=0, column=0)

        # Create a control panel frame.
        self.control_frame = tk.Frame(self.main_frame)
        self.control_frame.grid(row=1, column=0, sticky="ew", padx=5, pady=5)

        # Editor mode toggle.
        self.editor_mode = tk.BooleanVar(value=False)
        self.editor_mode_check = tk.Checkbutton(self.control_frame, text="Editor Mode", variable=self.editor_mode)
        self.editor_mode_check.grid(row=0, column=0, padx=5)

        # Editor action selection.
        self.editor_action = tk.StringVar(value="add")
        tk.Label(self.control_frame, text="Editor Action:").grid(row=0, column=1, padx=5)
        self.add_radio = tk.Radiobutton(self.control_frame, text="Add", variable=self.editor_action, value="add")
        self.add_radio.grid(row=0, column=2, padx=5)
        self.remove_radio = tk.Radiobutton(self.control_frame, text="Remove", variable=self.editor_action, value="remove")
        self.remove_radio.grid(row=0, column=3, padx=5)
        self.move_radio = tk.Radiobutton(self.control_frame, text="Move", variable=self.editor_action, value="move")
        self.move_radio.grid(row=0, column=4, padx=5)

        # When in "Add" mode, allow the user to choose a piece to add.
        tk.Label(self.control_frame, text="Piece to Add:").grid(row=0, column=5, padx=5)
        # Options: White pieces and Black pieces (use uppercase for white, lowercase for black)
        add_options = ["wP", "wR", "wN", "wB", "wQ", "wK", "bP", "bR", "bN", "bB", "bQ", "bK"]
        self.add_piece_choice = tk.StringVar(value="wP")
        self.add_option_menu = tk.OptionMenu(self.control_frame, self.add_piece_choice, *add_options)
        self.add_option_menu.grid(row=0, column=6, padx=5)

        # Initialize the chess board using python‑chess.
        self.board = chess.Board()
        # In editor mode, you might want to start with a clear board.
        # Uncomment the next line if you prefer starting with an empty board.
        # self.board.clear_board()

        # To track the selected square (for moves in game or editor "move" action).
        self.selected_square = None

        # Load custom piece images from SVG files.
        self.load_images()

        # Draw the board and pieces.
        self.draw_board()
        self.draw_pieces()

        # Bind click events on the canvas.
        self.canvas.bind("<Button-1>", self.on_click)

    def svg_to_photoimage(self, svg_path, size):
        """
        Convert an SVG file to a Tkinter PhotoImage.

        :param svg_path: Path to the SVG file.
        :param size: A tuple (width, height) to resize the output image.
        :return: A Tkinter PhotoImage.
        """
        try:
            # Convert the SVG file to PNG bytes using cairosvg.
            png_data = cairosvg.svg2png(url=svg_path, output_width=size[0], output_height=size[1])
        except Exception as e:
            print(f"Error converting {svg_path} to PNG: {e}")
            raise

        # Open the PNG bytes with Pillow and convert to a PhotoImage.
        image = Image.open(io.BytesIO(png_data))
        photo = ImageTk.PhotoImage(image)
        return photo

    def load_images(self):
        """
        Load custom icons from the "icons" directory.
        The keys are the piece symbols from python‑chess.
        White pieces are represented by uppercase letters, black by lowercase.
        """
        self.piece_images = {}
        pieces = ['P', 'R', 'N', 'B', 'Q', 'K']
        img_size = (SQUARE_SIZE, SQUARE_SIZE)

        for p in pieces:
            white_path = os.path.join("icons", f"w{p}.svg")
            black_path = os.path.join("icons", f"b{p}.svg")
            try:
                self.piece_images[p] = self.svg_to_photoimage(white_path, img_size)
                self.piece_images[p.lower()] = self.svg_to_photoimage(black_path, img_size)
            except Exception as e:
                print(f"Error loading image for piece {p}: {e}")
                raise

        # Keep references to the images so they are not garbage-collected.
        self.image_refs = list(self.piece_images.values())

    def draw_board(self):
        """Draw the chessboard squares."""
        self.squares = {}
        self.canvas.delete("square")
        for rank in range(8):
            for file in range(8):
                x1 = file * SQUARE_SIZE
                y1 = (7 - rank) * SQUARE_SIZE  # Flip rank for correct orientation.
                x2 = x1 + SQUARE_SIZE
                y2 = y1 + SQUARE_SIZE

                color = LIGHT_COLOR if (file + rank) % 2 == 0 else DARK_COLOR

                square_id = self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, tags="square")
                self.squares[(file, rank)] = square_id

    def draw_pieces(self):
        """Draw the chess pieces on the board using your custom icons."""
        self.canvas.delete("piece")
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is not None:
                file = chess.square_file(square)
                rank = chess.square_rank(square)
                x = file * SQUARE_SIZE + SQUARE_SIZE // 2
                y = (7 - rank) * SQUARE_SIZE + SQUARE_SIZE // 2
                image = self.piece_images[piece.symbol()]
                self.canvas.create_image(x, y, image=image, tags="piece")

    def highlight_square(self, file, rank):
        """Highlight the given square."""
        x1 = file * SQUARE_SIZE
        y1 = (7 - rank) * SQUARE_SIZE
        x2 = x1 + SQUARE_SIZE
        y2 = y1 + SQUARE_SIZE
        return self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=3, tags="highlight")

    def on_click(self, event):
        """Handle click events on the canvas."""
        file = event.x // SQUARE_SIZE
        rank = 7 - (event.y // SQUARE_SIZE)
        clicked_square = chess.square(file, rank)

        # Clear any previous highlight.
        self.canvas.delete("highlight")

        if self.editor_mode.get():
            # --- EDITOR MODE ---
            action = self.editor_action.get()

            if action == "remove":
                # Remove a piece if one exists.
                if self.board.piece_at(clicked_square) is not None:
                    self.board.remove_piece_at(clicked_square)
                    self.draw_board()
                    self.draw_pieces()
                # Clear any selected square.
                self.selected_square = None

            elif action == "add":
                # Add a piece if the square is empty.
                if self.board.piece_at(clicked_square) is None:
                    piece_symbol = self.add_piece_choice.get()
                    try:
                        # chess.Piece.from_symbol expects uppercase letter for white and lowercase for black.
                        piece = chess.Piece.from_symbol(piece_symbol)
                    except Exception as e:
                        print(f"Error creating piece from symbol {piece_symbol}: {e}")
                        return
                    self.board.set_piece_at(clicked_square, piece)
                    self.draw_board()
                    self.draw_pieces()

            elif action == "move":
                # In editor move mode, allow moving pieces without restrictions.
                if self.selected_square is None:
                    # If there's a piece on the clicked square, select it.
                    if self.board.piece_at(clicked_square) is not None:
                        self.selected_square = clicked_square
                        self.highlight_square(file, rank)
                else:
                    # Move piece from the selected square to the clicked square.
                    piece = self.board.piece_at(self.selected_square)
                    if piece is not None:
                        self.board.remove_piece_at(self.selected_square)
                        self.board.set_piece_at(clicked_square, piece)
                    self.draw_board()
                    self.draw_pieces()
                    self.selected_square = None

            # End of editor mode handling.
            return

        # --- GAME MODE (normal play using python‑chess logic) ---
        if self.selected_square is None:
            # Select a piece if one exists on the clicked square and if it is the side to move's piece.
            piece = self.board.piece_at(clicked_square)
            if piece and ((piece.symbol().isupper() and self.board.turn) or
                          (piece.symbol().islower() and not self.board.turn)):
                self.selected_square = clicked_square
                self.highlight_square(file, rank)
        else:
            # Attempt to make a move.
            move = chess.Move(self.selected_square, clicked_square)
            if move in self.board.legal_moves:
                self.board.push(move)
                self.draw_board()
                self.draw_pieces()
            else:
                print("Illegal move attempted:", move)
            self.selected_square = None

def main():
    root = tk.Tk()
    gui = ChessGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
