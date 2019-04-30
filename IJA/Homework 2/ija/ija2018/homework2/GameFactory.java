package ija.ija2018.homework2;

import ija.ija2018.homework2.game.Board;
import ija.ija2018.homework2.common.*;


public abstract class GameFactory {
    public GameFactory() {

    }


    public static Game createChessGame(Board board) {
       return new ChessGame(board);
    }



    public static Game createCheckersGame(Board board) {
        return new CheckersGame(board);
    }
}
