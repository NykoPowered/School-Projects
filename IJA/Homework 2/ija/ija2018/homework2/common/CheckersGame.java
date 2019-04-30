package ija.ija2018.homework2.common;

import ija.ija2018.homework2.game.Board;

import java.util.Stack;

public class CheckersGame implements Game {

    private static Figure[] PawnW=new Figure[12];

    private Stack<Figure> FigureStack = new Stack<>();
    private Stack<Field> Fieldstack = new Stack<>();

    public CheckersGame(Board board){
        for (int i = 0; i <= 7; i++) {
            if(((i+1) % 2) == 1) {
                Field f = board.getField(i + 1, 1);
                PawnW[i] = new Pawn(0, f);
                f.put(PawnW[i]);
            }
            else{
                Field f = board.getField(i + 1, 2);
                PawnW[i] = new Pawn(0, f);
                f.put(PawnW[i]);
            }
        }



    }

    public void undo(){

            Figure figure = FigureStack.pop();
            Field field = Fieldstack.pop();
            figure.getCoordinates().remove(figure);
            field.put(figure);
    }

    public boolean move(Figure figure, Field field) {

        int index = 0;
        if (figure.getRole() == Figure.Roles.Pawn && figure.getColor() == 0) {
            if (figure.getCoordinates().getrow() == 1) {
                return false;
            } else {
                if ((figure.getCoordinates().getcol() - field.getcol() == 1 || figure.getCoordinates().getcol() - field.getcol() == -1) && (field.getrow() - figure.getCoordinates().getrow() == 1)) {
                    Fieldstack.push(figure.getCoordinates());
                    FigureStack.push(figure);
                    figure.getCoordinates().remove(figure);
                    field.put(figure);
                    return true;
                } else {
                    return false;
                }
            }
        }
        return false;
    }

}
