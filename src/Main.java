import arduino.Arduino;
import arduino.PortDropdownMenu;
import com.fluxicon.slickerbox.components.SlickerButton;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Main {
    static Arduino arduino;
    static JFrame frame = new JFrame("IF Remote Controller");
    static SlickerButton btnPower = new SlickerButton("POWER");
    static SlickerButton volUp = new SlickerButton("VOL+");
    static SlickerButton volDown = new SlickerButton("VOL-");
    static SlickerButton prgUp = new SlickerButton("PROG+");
    static SlickerButton prgDown = new SlickerButton("PROG-");
    static SlickerButton btnRefresh;

    public static void main(String[] args) {
        setUpGUI();

        frame.setResizable(false);

        btnPower.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                arduino.serialWrite('P');

            }
        });
        volUp.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                arduino.serialWrite('U');
            }
        });
        volDown.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                arduino.serialWrite('D');
            }
        });
        prgUp.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                arduino.serialWrite('u');
            }
        });


    }

    public static void populateMenu() { //gets the list of available ports and fills the dropdown menu
        final PortDropdownMenu portList = new PortDropdownMenu();
        portList.refreshMenu();
        final SlickerButton connectButton = new SlickerButton("Connect");
        btnRefresh = new SlickerButton("refresh");

        JPanel topPanel = new JPanel();

        btnRefresh.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                portList.refreshMenu();

            }
        });
        topPanel.add(portList);
        topPanel.add(btnRefresh);
        topPanel.add(connectButton);
        // populate the drop-down box

        connectButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                if (connectButton.getText().equals("Connect")) {
                    arduino = new Arduino(portList.getSelectedItem().toString(), 9600);
                    if (arduino.openConnection()) {
                        connectButton.setText("Disconnect");
                        portList.setEnabled(false);
                        btnPower.setEnabled(true);
                        prgUp.setEnabled(true);
                        prgDown.setEnabled(true);
                        volUp.setEnabled(true);
                        volDown.setEnabled(true);
                        btnRefresh.setEnabled(false);
                        frame.pack();
                    }
                } else {
                    arduino.closeConnection();
                    connectButton.setText("Connect");
                    portList.setEnabled(true);
                    btnPower.setEnabled(false);
                    btnRefresh.setEnabled(true);
                }
            }

        });
        //topPanel.setBackground(Color.BLUE);
        frame.add(topPanel, BorderLayout.NORTH);
    }

    public static void setUpGUI() {
        frame.setSize(600, 600);
        frame.setBackground(Color.black);
        frame.setForeground(Color.black);
        frame.setPreferredSize(new Dimension(250, 250));
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLayout(new BorderLayout());
        btnPower.setForeground(Color.GREEN);
        btnPower.setEnabled(false);
        prgUp.setEnabled(false);
        prgDown.setEnabled(false);
        volUp.setEnabled(false);
        volDown.setEnabled(false);
        JPanel pane = new JPanel();
        pane.setLayout(new FlowLayout());
        //pane.setBackground(Color.blue);
        pane.add(btnPower);
        pane.add(prgUp);
        pane.add(prgDown);
        pane.add(volUp);
        pane.add(volDown);
        frame.add(pane, BorderLayout.CENTER);
        populateMenu();
        frame.pack();
        frame.getContentPane();
        frame.setVisible(true);

    }
}