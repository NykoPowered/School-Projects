/* IJA ukol 1 2019
    @author Nikolas Masica
 */

package ija.ija2018.homework1.board;

public class Board
{
    private Field[][] field;
    private int size;

    public Board(int size) {
        this.size = size;
        field = new Field[size+1][size+1];

        for (int col = 1; col <= size; col++) {
            for (int row = 1; row <= size; row++) {

                field[col][row] = new BoardField(col, row);
            }
        }

        for (int col = 1; col <= size; col++) {
            for (int row = 1; row <= size; row++) {

                if (col == 1 && row == 1) {
                    field[col][row].addNextField(Field.Direction.R, field[col + 1][row]);
                    field[col][row].addNextField(Field.Direction.RU, field[col + 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.U, field[col][row + 1]);
                } else if ((col == size) && (row == 1)) {
                    field[col][row].addNextField(Field.Direction.L, field[col - 1][row]);
                    field[col][row].addNextField(Field.Direction.LU, field[col - 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.U, field[col][row + 1]);
                } else if ((col == 1) && (row == size)) {
                    field[col][row].addNextField(Field.Direction.D, field[col][row - 1]);
                    field[col][row].addNextField(Field.Direction.R, field[col + 1][row]);
                    field[col][row].addNextField(Field.Direction.RD, field[col + 1][row - 1]);
                } else if ((col == size) && (row == size)) {
                    field[col][row].addNextField(Field.Direction.D, field[col][row - 1]);
                    field[col][row].addNextField(Field.Direction.L, field[col - 1][row]);
                    field[col][row].addNextField(Field.Direction.LD, field[col - 1][row - 1]);
                } else if ((col > 1) && (col < size) && (row == 1)) {
                    field[col][row].addNextField(Field.Direction.L, field[col - 1][row]);
                    field[col][row].addNextField(Field.Direction.LU, field[col - 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.R, field[col + 1][row]);
                    field[col][row].addNextField(Field.Direction.RU, field[col + 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.U, field[col][row + 1]);
                } else if ((col > 1) && (col < size) && (row == size)) {
                    field[col][row].addNextField(Field.Direction.D, field[col][row - 1]);
                    field[col][row].addNextField(Field.Direction.L, field[col - 1][row]);
                    field[col][row].addNextField(Field.Direction.LD, field[col - 1][row - 1]);
                    field[col][row].addNextField(Field.Direction.R, field[col + 1][row]);
                    field[col][row].addNextField(Field.Direction.RD, field[col + 1][row - 1]);
                } else if ((col == 1) && (row > 1) && (row < size)) {
                    field[col][row].addNextField(Field.Direction.D, field[col][row - 1]);
                    field[col][row].addNextField(Field.Direction.R, field[col + 1][row]);
                    field[col][row].addNextField(Field.Direction.RD, field[col + 1][row - 1]);
                    field[col][row].addNextField(Field.Direction.RU, field[col + 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.U, field[col][row + 1]);
                } else if ((col == size) && (row > 1) && (row < size)) {
                    field[col][row].addNextField(Field.Direction.D, field[col][row - 1]);
                    field[col][row].addNextField(Field.Direction.L, field[col - 1][row]);
                    field[col][row].addNextField(Field.Direction.LD, field[col - 1][row - 1]);
                    field[col][row].addNextField(Field.Direction.LU, field[col - 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.U, field[col][row + 1]);
                } else {
                    field[col][row].addNextField(Field.Direction.D, field[col][row - 1]);
                    field[col][row].addNextField(Field.Direction.L, field[col - 1][row]);
                    field[col][row].addNextField(Field.Direction.LD, field[col - 1][row - 1]);
                    field[col][row].addNextField(Field.Direction.LU, field[col - 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.R, field[col + 1][row]);
                    field[col][row].addNextField(Field.Direction.RD, field[col + 1][row - 1]);
                    field[col][row].addNextField(Field.Direction.RU, field[col + 1][row + 1]);
                    field[col][row].addNextField(Field.Direction.U, field[col][row + 1]);
                }
            }
        }
    }
    public Field getField (int col, int row)
    {
        return field[col][row];
    }
    public int getSize()
    {
        return size;
    }

}
