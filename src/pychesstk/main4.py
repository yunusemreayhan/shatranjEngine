import tkinter as tk
import chess
import io
import os

from PIL import Image, ImageTk
import cairosvg

# Colors for the board squares
LIGHT_COLOR = "#F0D9B5"
DARK_COLOR = "#B58863"


class ChessGUI:
    def __init__(self, master):
        self.master = master
        master.title("Resizable Chess GUI with Editor Mode and FEN Support")

        # We'll start with a default board size; it will update as the window resizes.
        self.board_size = 600  
        self.sq_count = 8  # 8x8 board
        self.square_size = self.board_size // self.sq_count

        # Create a container frame for board and control panel.
        self.main_frame = tk.Frame(master)
        self.main_frame.grid(row=0, column=0, sticky="nsew")
        master.grid_rowconfigure(0, weight=1)
        master.grid_columnconfigure(0, weight=1)

        # Create a canvas for the chess board.
        self.canvas = tk.Canvas(self.main_frame, width=self.board_size, height=self.board_size, bg="gray")
        self.canvas.grid(row=0, column=0, sticky="nsew")

        # Bind canvas resize event.
        self.canvas.bind("<Configure>", self.on_canvas_resize)

        # Create a control panel frame below the board.
        self.control_frame = tk.Frame(self.main_frame, bd=2, relief=tk.SUNKEN)
        self.control_frame.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)

        # Configure grid weights so that both board and control panel expand.
        self.main_frame.rowconfigure(0, weight=3)  # Board gets more space
        self.main_frame.rowconfigure(1, weight=1)  # Control panel gets less (but still expands)
        self.main_frame.columnconfigure(0, weight=1)

        # --- Control Panel Widgets ---
        # Editor mode toggle.
        self.editor_mode = tk.BooleanVar(value=False)
        self.editor_mode_check = tk.Checkbutton(self.control_frame, text="Editor Mode", variable=self.editor_mode)
        self.editor_mode_check.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        # Editor action selection.
        self.editor_action = tk.StringVar(value="add")
        tk.Label(self.control_frame, text="Editor Action:").grid(row=0, column=1, padx=5, pady=5, sticky="ew")
        self.add_radio = tk.Radiobutton(self.control_frame, text="Add", variable=self.editor_action, value="add")
        self.add_radio.grid(row=0, column=2, padx=5, pady=5, sticky="ew")
        self.remove_radio = tk.Radiobutton(self.control_frame, text="Remove", variable=self.editor_action, value="remove")
        self.remove_radio.grid(row=0, column=3, padx=5, pady=5, sticky="ew")
        self.move_radio = tk.Radiobutton(self.control_frame, text="Move", variable=self.editor_action, value="move")
        self.move_radio.grid(row=0, column=4, padx=5, pady=5, sticky="ew")

        # Option to choose which piece to add.
        tk.Label(self.control_frame, text="Piece to Add:").grid(row=0, column=5, padx=5, pady=5, sticky="ew")
        add_options = ["wP", "wR", "wN", "wB", "wQ", "wK", "bP", "bR", "bN", "bB", "bQ", "bK"]
        self.add_piece_choice = tk.StringVar(value="wP")
        self.add_option_menu = tk.OptionMenu(self.control_frame, self.add_piece_choice, *add_options)
        self.add_option_menu.grid(row=0, column=6, padx=5, pady=5, sticky="ew")

        # FEN string support controls.
        fen_frame = tk.Frame(self.control_frame)
        fen_frame.grid(row=1, column=0, columnspan=7, sticky="nsew", padx=5, pady=5)
        tk.Label(fen_frame, text="FEN:").grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        self.fen_entry = tk.Entry(fen_frame, width=50)
        self.fen_entry.grid(row=0, column=1, padx=5, pady=5, sticky="ew")
        load_fen_btn = tk.Button(fen_frame, text="Load FEN", command=self.load_fen)
        load_fen_btn.grid(row=0, column=2, padx=5, pady=5, sticky="ew")
        show_fen_btn = tk.Button(fen_frame, text="Show FEN", command=self.show_fen)
        show_fen_btn.grid(row=0, column=3, padx=5, pady=5, sticky="ew")

        # Make fen_frame columns expand proportionally.
        fen_frame.columnconfigure(1, weight=1)

        # Initialize a python‑chess board.
        self.board = chess.Board()
        # Optionally, you can start with an empty board:
        # self.board.clear_board()

        # To track a selected square (for both game mode and editor "move" action).
        self.selected_square = None

        # Load piece images based on the current square size.
        self.load_images()

        # Draw board and pieces initially.
        self.draw_board()
        self.draw_pieces()

        # Bind click events on the canvas.
        self.canvas.bind("<Button-1>", self.on_click)

    def on_canvas_resize(self, event):
        """Handle canvas resize events so that the board remains square and scales its contents."""
        # Determine the new size as the minimum of the canvas's width and height.
        new_size = min(event.width, event.height)
        if new_size != self.board_size:
            self.board_size = new_size
            self.square_size = self.board_size // self.sq_count

            # Resize the canvas to a square.
            self.canvas.config(width=self.board_size, height=self.board_size)

            # Reload images at the new size.
            self.load_images()

            # Redraw board and pieces.
            self.draw_board()
            self.draw_pieces()

    def svg_to_photoimage(self, svg_path, size):
        """
        Convert an SVG file to a Tkinter PhotoImage at a given size.

        :param svg_path: Path to the SVG file.
        :param size: Tuple (width, height) for the output image.
        :return: Tkinter PhotoImage.
        """
        try:
            png_data = cairosvg.svg2png(url=svg_path, output_width=size[0], output_height=size[1])
        except Exception as e:
            print(f"Error converting {svg_path} to PNG: {e}")
            raise

        image = Image.open(io.BytesIO(png_data))
        return ImageTk.PhotoImage(image)

    def load_images(self):
        """Load and (re)create piece images at the current square size."""
        self.piece_images = {}
        pieces = ['P', 'R', 'N', 'B', 'Q', 'K']
        img_size = (self.square_size, self.square_size)

        for p in pieces:
            white_path = os.path.join("icons", f"w{p}.svg")
            black_path = os.path.join("icons", f"b{p}.svg")
            try:
                self.piece_images[p] = self.svg_to_photoimage(white_path, img_size)
                self.piece_images[p.lower()] = self.svg_to_photoimage(black_path, img_size)
            except Exception as e:
                print(f"Error loading image for piece {p}: {e}")
                raise

        # Keep a reference list to avoid garbage collection.
        self.image_refs = list(self.piece_images.values())

    def draw_board(self):
        """Redraw the chessboard squares."""
        self.canvas.delete("square")
        for rank in range(self.sq_count):
            for file in range(self.sq_count):
                x1 = file * self.square_size
                y1 = (self.sq_count - 1 - rank) * self.square_size
                x2 = x1 + self.square_size
                y2 = y1 + self.square_size
                color = LIGHT_COLOR if (file + rank) % 2 == 0 else DARK_COLOR
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="", tags="square")

    def draw_pieces(self):
        """Redraw the chess pieces on the board."""
        self.canvas.delete("piece")
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is not None:
                file = chess.square_file(square)
                rank = chess.square_rank(square)
                x = file * self.square_size + self.square_size // 2
                y = (self.sq_count - 1 - rank) * self.square_size + self.square_size // 2
                image = self.piece_images[piece.symbol()]
                self.canvas.create_image(x, y, image=image, tags="piece")

    def highlight_square(self, file, rank):
        """Highlight a given square with a blue border."""
        x1 = file * self.square_size
        y1 = (self.sq_count - 1 - rank) * self.square_size
        x2 = x1 + self.square_size
        y2 = y1 + self.square_size
        return self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=3, tags="highlight")

    def load_fen(self):
        """Load the board position from the FEN string in the entry field."""
        fen = self.fen_entry.get().strip()
        try:
            self.board.set_fen(fen)
            self.draw_board()
            self.draw_pieces()
        except Exception as e:
            print("Invalid FEN:", e)

    def show_fen(self):
        """Display the current board's FEN string in the entry field."""
        fen = self.board.fen()
        self.fen_entry.delete(0, tk.END)
        self.fen_entry.insert(0, fen)

    def on_click(self, event):
        """Handle mouse clicks on the canvas."""
        file = event.x // self.square_size
        rank = self.sq_count - 1 - (event.y // self.square_size)
        clicked_square = chess.square(file, rank)

        # Clear any previous highlight.
        self.canvas.delete("highlight")

        if self.editor_mode.get():
            action = self.editor_action.get()
            if action == "remove":
                if self.board.piece_at(clicked_square) is not None:
                    self.board.remove_piece_at(clicked_square)
                    self.draw_board()
                    self.draw_pieces()
                self.selected_square = None
            elif action == "add":
                if self.board.piece_at(clicked_square) is None:
                    piece_symbol = self.add_piece_choice.get()
                    try:
                        # Create piece: note that chess.Piece.from_symbol expects a letter
                        # so we extract the letter from e.g. "wP" or "bK" and then set color accordingly.
                        piece_type = chess.Piece.from_symbol(piece_symbol[1]).piece_type
                        color = chess.WHITE if piece_symbol.startswith("w") else chess.BLACK
                        piece = chess.Piece(piece_type, color)
                    except Exception as e:
                        print(f"Error creating piece from symbol {piece_symbol}: {e}")
                        return
                    self.board.set_piece_at(clicked_square, piece)
                    self.draw_board()
                    self.draw_pieces()
            elif action == "move":
                if self.selected_square is None:
                    if self.board.piece_at(clicked_square) is not None:
                        self.selected_square = clicked_square
                        self.highlight_square(file, rank)
                else:
                    piece = self.board.piece_at(self.selected_square)
                    if piece is not None:
                        self.board.remove_piece_at(self.selected_square)
                        self.board.set_piece_at(clicked_square, piece)
                    self.draw_board()
                    self.draw_pieces()
                    self.selected_square = None
            return  # End editor mode handling

        # --- GAME MODE ---
        if self.selected_square is None:
            piece = self.board.piece_at(clicked_square)
            if piece and ((piece.symbol().isupper() and self.board.turn) or
                          (piece.symbol().islower() and not self.board.turn)):
                self.selected_square = clicked_square
                self.highlight_square(file, rank)
        else:
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
    # Allow the window to be resizable.
    root.rowconfigure(0, weight=1)
    root.columnconfigure(0, weight=1)
    gui = ChessGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
