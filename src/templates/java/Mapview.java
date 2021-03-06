/** Generated by Semantik */

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.border.*;
import javax.swing.colorchooser.*;
import javax.swing.filechooser.*;
import javax.accessibility.*;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import java.io.*;
import java.applet.*;
import java.net.*;

class Mapview extends JScrollPane implements MouseListener, MouseMotionListener
{
	Point m_oldposition;
	Point m_initclick;

	double m_picwidth;
	double m_picheight;

	Mapview()
	{
		super();

		// Panel to hold the map picture
		JPanel panel = new JPanel(new BorderLayout());

		String path = "/pics/wholemap.png";
		ImageIcon pic = new ImageIcon(getClass().getResource(path));

		m_picwidth = pic.getIconWidth();
		m_picheight = pic.getIconHeight();

		panel.add(new JLabel(pic), BorderLayout.CENTER);
		getViewport().add(panel);

		JScrollBar vsb = getVerticalScrollBar();
		JScrollBar hsb = getHorizontalScrollBar();

		vsb.setValue(pic.getIconHeight());
		hsb.setValue(pic.getIconWidth());

		addMouseListener(this);
		addMouseMotionListener(this);
	}

	public void mouseExited(MouseEvent e) { }

	public void mouseEntered(MouseEvent e) { }

	public void mouseReleased(MouseEvent e) { }

	public void mousePressed(MouseEvent e)
	{
		m_initclick = e.getPoint();
		m_oldposition = getViewport().getViewPosition();
	}

	public void mouseClicked(MouseEvent e) { }

	public void mouseMoved(MouseEvent e)
	{
		Rectangle r = getViewport().getViewRect();

		// will not work with zoom
		double x = e.getPoint().getX() + r.getX();
		double y = e.getPoint().getY() + r.getY();

		String text = TextCoord.coordToText(x, y);
		/*if (text==null || text.length() < 1)
		{
			text = ""+x+" "+y;
		}*/
		setToolTipText(text);
	}

	public void mouseDragged(MouseEvent e)
	{
		double x = m_oldposition.getX() + m_initclick.getX() - e.getPoint().getX();
		double y = m_oldposition.getY() + m_initclick.getY() - e.getPoint().getY();

		Rectangle r = getViewport().getViewRect();

		if (x < 0) x=0;
		if (y < 0) y=0;

		if (x > m_picwidth - r.getWidth()) x = m_picwidth - r.getWidth();
		if (y > m_picheight - r.getHeight()) y = m_picheight - r.getHeight();

		Point p = new Point();
		p.setLocation( x, y );
		getViewport().setViewPosition( p );
	}
}

