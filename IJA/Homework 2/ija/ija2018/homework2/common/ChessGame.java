package ija.ija2018.homework2.common;
import ija.ija2018.homework2.game.Board;
import java.util.Stack;

public class ChessGame implements Game {
    private static Figure[] PawnW=new Figure[8];
    private static Figure[] PawnB=new Figure[8];
    private static Figure[] RookW=new Figure[2];
    private static Figure[] RookB=new Figure[2];

    private Stack<Figure> FigureStack = new Stack<>();
    private Stack<Field> Fieldstack = new Stack<>();
    private Stack<Integer> IntegerStack = new Stack<>();

    public ChessGame(Board board){
        for (int i =0; i<=7; i++){
            Field f = board.getField(i+1,2);
            PawnW[i] = new Pawn(0,f);
            f.put(PawnW[i]);
        }

        for (int i =0; i<=7; i++){
            Field f = board.getField(i+1,7);
            PawnB[i] = new Pawn(1,f);
            f.put(PawnB[i]);
        }

        Field f2 = board.getField(1,1);
        RookW[0]= new Rook(0,f2);
        f2.put(RookW[0]);

        f2 = board.getField(8,1);
        RookW[1]= new Rook(0,f2);
        f2.put(RookW[1]);

        f2 = board.getField(1,8);
        RookB[0]= new Rook(1,f2);
        f2.put(RookB[0]);

        f2 = board.getField(8,8);
        RookB[1]= new Rook(1,f2);
        f2.put(RookB[1]);

    }

    public void undo(){
        int zmena= IntegerStack.pop();
        if(zmena==0){
            Figure figure = FigureStack.pop();
            Field field = Fieldstack.pop();
            figure.getCoordinates().remove(figure);
            field.put(figure);
        }
        else if(zmena == 1){
            Figure figure_pop = FigureStack.pop();
            Field field_pop = Fieldstack.pop();
            Figure figure = FigureStack.pop();
            Field field = Fieldstack.pop();

            figure.getCoordinates().remove(figure);
            field.put(figure);
            field_pop.put(figure_pop);
        }
    }

    public boolean move(Figure figure, Field field){
        FigureStack.push(figure);
        Fieldstack.push(figure.getCoordinates());
        int index = 0;
        if(figure.getRole() == Figure.Roles.Pawn && figure.getColor() == 0){
            if(figure.getCoordinates().getcol() == field.getcol() && (field.getrow() - figure.getCoordinates().getrow() == 1)){
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            }else{
                return false;
            }
        }
        else if(figure.getRole() == Figure.Roles.Pawn && figure.getColor() == 1)
        {
            if (figure.getCoordinates().getcol() == field.getcol() && (figure.getCoordinates().getrow() - field.getrow() == 1)) {
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else {
                return false;
            }
        }
        else if(figure.getRole() == Figure.Roles.Rook && figure.getColor() == 0) {
            Field help = figure.getCoordinates();
            if (figure.getCoordinates().getcol() > field.getcol() && figure.getCoordinates().getrow() == field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.L).isEmpty()) {
                        if (help.getcol() - 1 == field.getcol() && field.get().getColor() == 1) {
                            index = 1;
                            help = help.nextField(Field.Direction.L);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.L);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else if (figure.getCoordinates().getcol() < field.getcol() && figure.getCoordinates().getrow() == field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.R).isEmpty()) {
                        if (help.getcol() + 1 == field.getcol() && field.get().getColor() == 1) {
                            index = 1;
                            help = help.nextField(Field.Direction.R);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.R);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else if (figure.getCoordinates().getcol() == field.getcol() && figure.getCoordinates().getrow() < field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.U).isEmpty()) {
                        if (help.getrow() + 1 == field.getrow() && field.get().getColor() == 1) {
                            index = 1;
                            help = help.nextField(Field.Direction.U);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.U);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else if (figure.getCoordinates().getcol() == field.getcol() && figure.getCoordinates().getrow() > field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.D).isEmpty()) {
                        if (help.getrow() - 1 == field.getrow() && field.get().getColor() == 1) {
                            index = 1;
                            help = help.nextField(Field.Direction.D);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.D);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            }
        }else if(figure.getRole() == Figure.Roles.Rook && figure.getColor() == 1) {
            Field help = figure.getCoordinates();
            if (figure.getCoordinates().getcol() > field.getcol() && figure.getCoordinates().getrow() == field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.L).isEmpty()) {
                        if (help.getcol() - 1 == field.getcol() && field.get().getColor() == 0) {
                            index = 1;
                            help = help.nextField(Field.Direction.L);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.L);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else if (figure.getCoordinates().getcol() < field.getcol() && figure.getCoordinates().getrow() == field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.R).isEmpty()) {
                        if (help.getcol() + 1 == field.getcol() && field.get().getColor() == 0) {
                            index = 1;
                            help = help.nextField(Field.Direction.R);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.R);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else if (figure.getCoordinates().getcol() == field.getcol() && figure.getCoordinates().getrow() < field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.U).isEmpty()) {
                        if (help.getrow() + 1 == field.getrow() && field.get().getColor() == 0) {
                            index = 1;
                            help = help.nextField(Field.Direction.U);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.U);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            } else if (figure.getCoordinates().getcol() == field.getcol() && figure.getCoordinates().getrow() > field.getrow()) {
                while (help != field) {
                    if (!help.nextField(Field.Direction.D).isEmpty()) {
                        if (help.getrow() - 1 == field.getrow() && field.get().getColor() == 0) {
                            index = 1;
                            help = help.nextField(Field.Direction.D);
                            FigureStack.push(field.get());
                            Fieldstack.push(field);
                            field.remove(field.get());
                        } else {
                            return false;
                        }
                    } else {
                        help = help.nextField(Field.Direction.D);
                    }
                }
                IntegerStack.push(index);
                figure.getCoordinates().remove(figure);
                field.put(figure);
                return true;
            }
        }
        else{
            FigureStack.pop();
            Fieldstack.pop();
            return false;
        }
        return false;
    }


}
