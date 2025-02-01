import tkinter as tk
import chess
import io
import os
import datetime

from PIL import Image, ImageTk
import cairosvg

# Colors for board squares.
LIGHT_COLOR = "#F0D9B5"
DARK_COLOR = "#B58863"


class ChessGUI:
    def __init__(self, master):
        self.master = master
        master.title("Chaturanga Chess GUI")

        # Initialize move history (list of algebraic–like move strings).
        self.move_history_list = []

        # Create a container frame with two columns:
        # Left: board & controls, Right: move history and extra buttons.
        self.container = tk.Frame(master)
        self.container.grid(row=0, column=0, sticky="nsew")
        master.rowconfigure(0, weight=1)
        master.columnconfigure(0, weight=1)

        # Left frame holds the board and control panel.
        self.left_frame = tk.Frame(self.container)
        self.left_frame.grid(row=0, column=0, sticky="nsew")

        # Right frame holds the move history and extra buttons.
        self.history_frame = tk.Frame(self.container, bd=2, relief=tk.SUNKEN)
        self.history_frame.grid(row=0, column=1, sticky="nsew", padx=5, pady=5)

        # Configure container grid weights.
        self.container.columnconfigure(0, weight=3)
        self.container.columnconfigure(1, weight=1)
        self.container.rowconfigure(0, weight=1)

        # Board settings.
        self.board_size = 600
        self.sq_count = 8
        self.square_size = self.board_size // self.sq_count

        # Create the chess board canvas.
        self.canvas = tk.Canvas(self.left_frame, width=self.board_size, height=self.board_size, bg="gray")
        self.canvas.grid(row=0, column=0, sticky="nsew")
        self.canvas.bind("<Configure>", self.on_canvas_resize)

        # Create the control panel (below the board).
        self.control_frame = tk.Frame(self.left_frame, bd=2, relief=tk.SUNKEN)
        self.control_frame.grid(row=1, column=0, sticky="ew", padx=5, pady=5)
        self.left_frame.rowconfigure(0, weight=3)
        self.left_frame.rowconfigure(1, weight=1)
        self.left_frame.columnconfigure(0, weight=1)

        # --- Control Panel Widgets ---

        # Editor Mode Toggle.
        self.editor_mode = tk.BooleanVar(value=False)
        self.editor_mode_check = tk.Checkbutton(self.control_frame, text="Editor Mode", variable=self.editor_mode)
        self.editor_mode_check.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        # Editor Action Selection.
        self.editor_action = tk.StringVar(value="add")
        tk.Label(self.control_frame, text="Editor Action:").grid(row=0, column=1, padx=5, pady=5, sticky="ew")
        self.add_radio = tk.Radiobutton(self.control_frame, text="Add", variable=self.editor_action, value="add")
        self.add_radio.grid(row=0, column=2, padx=5, pady=5, sticky="ew")
        self.remove_radio = tk.Radiobutton(self.control_frame, text="Remove", variable=self.editor_action, value="remove")
        self.remove_radio.grid(row=0, column=3, padx=5, pady=5, sticky="ew")
        self.move_radio = tk.Radiobutton(self.control_frame, text="Move", variable=self.editor_action, value="move")
        self.move_radio.grid(row=0, column=4, padx=5, pady=5, sticky="ew")

        # Piece-to-Add Option.
        tk.Label(self.control_frame, text="Piece to Add:").grid(row=0, column=5, padx=5, pady=5, sticky="ew")
        add_options = ["wP", "wR", "wN", "wB", "wQ", "wK",
                       "bP", "bR", "bN", "bB", "bQ", "bK"]
        self.add_piece_choice = tk.StringVar(value="wP")
        self.add_option_menu = tk.OptionMenu(self.control_frame, self.add_piece_choice, *add_options)
        self.add_option_menu.grid(row=0, column=6, padx=5, pady=5, sticky="ew")

        # Moving Side Selection.
        tk.Label(self.control_frame, text="Side to Move:").grid(row=0, column=7, padx=5, pady=5, sticky="ew")
        self.side_var = tk.StringVar(value="White")
        self.side_menu = tk.OptionMenu(self.control_frame, self.side_var, "White", "Black", command=self.on_side_change)
        self.side_menu.grid(row=0, column=8, padx=5, pady=5, sticky="ew")

        # FEN Controls (in a new row).
        fen_frame = tk.Frame(self.control_frame)
        fen_frame.grid(row=1, column=0, columnspan=9, sticky="ew", padx=5, pady=5)
        tk.Label(fen_frame, text="FEN:").grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        self.fen_entry = tk.Entry(fen_frame, width=50)
        self.fen_entry.grid(row=0, column=1, padx=5, pady=5, sticky="ew")
        load_fen_btn = tk.Button(fen_frame, text="Load FEN", command=self.load_fen)
        load_fen_btn.grid(row=0, column=2, padx=5, pady=5, sticky="ew")
        show_fen_btn = tk.Button(fen_frame, text="Show FEN", command=self.show_fen)
        show_fen_btn.grid(row=0, column=3, padx=5, pady=5, sticky="ew")
        fen_frame.columnconfigure(1, weight=1)

        # Initialize the python‑chess board.
        self.board = chess.Board()  # Standard starting position.
        self.board.turn = chess.WHITE  # Default moving side is White.

        # Track selected square for moves.
        self.selected_square = None

        # Load piece images and draw board.
        self.load_images()
        self.draw_board()
        self.draw_pieces()

        # Bind mouse clicks on the board.
        self.canvas.bind("<Button-1>", self.on_click)

        # Set up the move history panel (right side).
        self.setup_history_panel()

    def setup_history_panel(self):
        """Set up the move history panel with a text widget and extra buttons."""
        header = tk.Label(self.history_frame, text="Move History", font=("Helvetica", 14))
        header.pack(padx=5, pady=5)
        self.history_text = tk.Text(self.history_frame, width=30, state="disabled")
        self.history_text.pack(fill="both", expand=True, padx=5, pady=5)

        # Clear History button.
        clear_history_btn = tk.Button(self.history_frame, text="Clear History", command=self.clear_history)
        clear_history_btn.pack(padx=5, pady=5, fill="x")

        # Additional buttons.
        clear_board_btn = tk.Button(self.history_frame, text="Clear Board", command=self.clear_board)
        clear_board_btn.pack(padx=5, pady=5, fill="x")
        starting_position_btn = tk.Button(self.history_frame, text="Starting Position", command=self.starting_position)
        starting_position_btn.pack(padx=5, pady=5, fill="x")
        export_pgn_btn = tk.Button(self.history_frame, text="Export PGN", command=self.export_pgn)
        export_pgn_btn.pack(padx=5, pady=5, fill="x")

    def clear_history(self):
        """Clear the move history text widget and list."""
        self.history_text.config(state="normal")
        self.history_text.delete("1.0", tk.END)
        self.history_text.config(state="disabled")
        self.move_history_list = []

    def add_move_history(self, move_str):
        """Append a move to the move history panel and list."""
        self.move_history_list.append(move_str)
        self.history_text.config(state="normal")
        self.history_text.insert(tk.END, move_str + " ")
        # Insert a newline after black's move.
        if len(self.move_history_list) % 2 == 0:
            self.history_text.insert(tk.END, "\n")
        self.history_text.see(tk.END)
        self.history_text.config(state="disabled")

    def export_pgn(self):
        """Generate a PGN text from the move history and display it in a popup."""
        current_date = datetime.datetime.now().strftime("%Y.%m.%d")
        headers = [
            '[Event "Chaturanga Game"]',
            '[Site "Local"]',
            f'[Date "{current_date}"]',
            '[Round "-"]',
            '[White "White"]',
            '[Black "Black"]',
            '[Result "*"]',
            ''
        ]
        # Group moves by pair (white then black).
        pgn_moves = []
        move_number = 1
        moves = self.move_history_list
        for i in range(0, len(moves), 2):
            white_move = moves[i]
            black_move = moves[i + 1] if i + 1 < len(moves) else ""
            if black_move:
                pgn_moves.append(f"{move_number}. {white_move} {black_move}")
            else:
                pgn_moves.append(f"{move_number}. {white_move}")
            move_number += 1
        pgn_text = "\n".join(headers + pgn_moves)
        popup = tk.Toplevel(self.master)
        popup.title("Exported PGN")
        text_widget = tk.Text(popup, width=50, height=20)
        text_widget.pack(expand=True, fill="both")
        text_widget.insert("1.0", pgn_text)
        text_widget.config(state="disabled")

    def clear_board(self):
        """Clear the board (remove all pieces) and update display and history."""
        self.board.clear_board()
        self.draw_board()
        self.draw_pieces()
        self.clear_history()

    def starting_position(self):
        """Reset the board to the starting position and update display and history."""
        self.board = chess.Board()
        self.board.turn = chess.WHITE
        self.side_var.set("White")
        self.draw_board()
        self.draw_pieces()
        self.clear_history()

    def on_side_change(self, value):
        """Update the board's turn when the moving side selection changes."""
        self.board.turn = chess.WHITE if value == "White" else chess.BLACK

    def on_canvas_resize(self, event):
        """Keep the board square and resize images when the canvas size changes."""
        new_size = min(event.width, event.height)
        if new_size != self.board_size:
            self.board_size = new_size
            self.square_size = self.board_size // self.sq_count
            self.canvas.config(width=self.board_size, height=self.board_size)
            self.load_images()
            self.draw_board()
            self.draw_pieces()

    def svg_to_photoimage(self, svg_path, size):
        """Convert an SVG file to a PhotoImage of the specified size."""
        try:
            png_data = cairosvg.svg2png(url=svg_path, output_width=size[0], output_height=size[1])
        except Exception as e:
            print(f"Error converting {svg_path} to PNG: {e}")
            raise
        image = Image.open(io.BytesIO(png_data))
        return ImageTk.PhotoImage(image)

    def load_images(self):
        """Load (or reload) piece images scaled to the current square size."""
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
        # Keep a reference to avoid garbage collection.
        self.image_refs = list(self.piece_images.values())

    def draw_board(self):
        """Draw the board squares."""
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
        """Draw the pieces on the board."""
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
        """Highlight a square with a blue border."""
        x1 = file * self.square_size
        y1 = (self.sq_count - 1 - rank) * self.square_size
        x2 = x1 + self.square_size
        y2 = y1 + self.square_size
        return self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=3, tags="highlight")

    def load_fen(self):
        """Load a board position from a FEN string."""
        fen = self.fen_entry.get().strip()
        try:
            self.board.set_fen(fen)
            self.draw_board()
            self.draw_pieces()
        except Exception as e:
            print("Invalid FEN:", e)

    def show_fen(self):
        """Show the current board's FEN in the entry field."""
        fen = self.board.fen()
        self.fen_entry.delete(0, tk.END)
        self.fen_entry.insert(0, fen)

    def get_algebraic_move(self, from_sq, to_sq, capture):
        """
        Return a simple algebraic–like notation for a move.
        
        For pawn moves, a non–capture is simply the destination (e.g. "e4"),
        while a capture is given as file + "x" + destination (e.g. "exd5").
        For other pieces, a capital letter is prefixed (e.g. "Nf3" or "Nxf3").
        """
        piece = self.board.piece_at(from_sq)
        if piece is None:
            return ""
        from_name = chess.square_name(from_sq)
        to_name = chess.square_name(to_sq)
        if piece.piece_type == chess.PAWN:
            if capture:
                return from_name[0] + "x" + to_name
            else:
                return to_name
        else:
            letter = {chess.KNIGHT: "N", chess.BISHOP: "B",
                      chess.ROOK: "R", chess.QUEEN: "Q",
                      chess.KING: "K"}.get(piece.piece_type, "")
            if capture:
                return letter + "x" + to_name
            else:
                return letter + to_name

    def legal_moves_for_piece(self, square):
        """
        Generate destination squares for a piece on the given square
        following Chaturanga rules.
        """
        piece = self.board.piece_at(square)
        if piece is None:
            return []
        color = piece.color
        file = chess.square_file(square)
        rank = chess.square_rank(square)
        moves = []

        if piece.piece_type == chess.PAWN:
            # Pawn moves one square forward; diagonal capture if an enemy occupies that square.
            direction = 1 if color == chess.WHITE else -1
            forward_rank = rank + direction
            if 0 <= forward_rank < 8:
                forward_sq = chess.square(file, forward_rank)
                if self.board.piece_at(forward_sq) is None:
                    moves.append(forward_sq)
                for df in [-1, 1]:
                    new_file = file + df
                    if 0 <= new_file < 8:
                        diag_sq = chess.square(new_file, forward_rank)
                        target = self.board.piece_at(diag_sq)
                        if target is not None and target.color != color:
                            moves.append(diag_sq)

        elif piece.piece_type == chess.KNIGHT:
            knight_moves = [(2, 1), (1, 2), (-1, 2), (-2, 1),
                            (-2, -1), (-1, -2), (1, -2), (2, -1)]
            for df, dr in knight_moves:
                new_file = file + df
                new_rank = rank + dr
                if 0 <= new_file < 8 and 0 <= new_rank < 8:
                    sq = chess.square(new_file, new_rank)
                    target = self.board.piece_at(sq)
                    if target is None or target.color != color:
                        moves.append(sq)

        elif piece.piece_type == chess.KING:
            for df in [-1, 0, 1]:
                for dr in [-1, 0, 1]:
                    if df == 0 and dr == 0:
                        continue
                    new_file = file + df
                    new_rank = rank + dr
                    if 0 <= new_file < 8 and 0 <= new_rank < 8:
                        sq = chess.square(new_file, new_rank)
                        target = self.board.piece_at(sq)
                        if target is None or target.color != color:
                            moves.append(sq)

        elif piece.piece_type == chess.ROOK:
            directions = [(1, 0), (-1, 0), (0, 1), (0, -1)]
            for df, dr in directions:
                new_file = file
                new_rank = rank
                while True:
                    new_file += df
                    new_rank += dr
                    if not (0 <= new_file < 8 and 0 <= new_rank < 8):
                        break
                    sq = chess.square(new_file, new_rank)
                    target = self.board.piece_at(sq)
                    if target is None:
                        moves.append(sq)
                    else:
                        if target.color != color:
                            moves.append(sq)
                        break

        elif piece.piece_type == chess.BISHOP:
            # Elephant moves exactly two squares diagonally, jumping over the intermediate square.
            for df, dr in [(1, 1), (1, -1), (-1, 1), (-1, -1)]:
                new_file = file + 2 * df
                new_rank = rank + 2 * dr
                if 0 <= new_file < 8 and 0 <= new_rank < 8:
                    sq = chess.square(new_file, new_rank)
                    target = self.board.piece_at(sq)
                    if target is None or target.color != color:
                        moves.append(sq)

        elif piece.piece_type == chess.QUEEN:
            # Mantri moves one square diagonally.
            for df, dr in [(1, 1), (1, -1), (-1, 1), (-1, -1)]:
                new_file = file + df
                new_rank = rank + dr
                if 0 <= new_file < 8 and 0 <= new_rank < 8:
                    sq = chess.square(new_file, new_rank)
                    target = self.board.piece_at(sq)
                    if target is None or target.color != color:
                        moves.append(sq)

        return moves

    def on_click(self, event):
        """Handle mouse click events on the canvas."""
        file = event.x // self.square_size
        rank = self.sq_count - 1 - (event.y // self.square_size)
        clicked_square = chess.square(file, rank)
        self.canvas.delete("highlight")

        # --- Editor Mode ---
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
                        # Create a piece from its symbol (e.g. "wP" or "bK").
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
            return  # End editor mode handling.

        # --- Game Mode (Chaturanga moves) ---
        if self.selected_square is None:
            piece = self.board.piece_at(clicked_square)
            if piece and ((piece.symbol().isupper() and self.board.turn) or
                          (piece.symbol().islower() and not self.board.turn)):
                self.selected_square = clicked_square
                self.highlight_square(file, rank)
        else:
            legal = self.legal_moves_for_piece(self.selected_square)
            if clicked_square in legal:
                # Determine if a capture occurs.
                captured_piece = self.board.piece_at(clicked_square)
                move_str = self.get_algebraic_move(self.selected_square, clicked_square, captured_piece is not None)
                piece = self.board.piece_at(self.selected_square)
                self.board.remove_piece_at(self.selected_square)
                self.board.set_piece_at(clicked_square, piece)
                self.add_move_history(move_str)
                self.draw_board()
                self.draw_pieces()
                # Toggle turn and update side selection.
                self.board.turn = not self.board.turn
                self.side_var.set("White" if self.board.turn else "Black")
            else:
                print("Illegal move attempted:", chess.square_name(self.selected_square), "->", chess.square_name(clicked_square))
            self.selected_square = None


def main():
    root = tk.Tk()
    root.rowconfigure(0, weight=1)
    root.columnconfigure(0, weight=1)
    gui = ChessGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
