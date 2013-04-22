<?php
/******************************************************************************
 * Copyright (c) 2010 Jevon Wright and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Jevon Wright - initial API and implementation
 ****************************************************************************/

/**
 * Tries to convert the given HTML into a plain text format - best suited for
 * e-mail display, etc.
 *
 * <p>In particular, it tries to maintain the following features:
 * <ul>
 *   <li>Links are maintained, with the 'href' copied over
 *   <li>Information in the &lt;head&gt; is lost
 * </ul>
 *
 * @param html the input HTML
 * @return the HTML converted, as best as possible, to text
 */
function convert_html_to_text($html) {
	$tags = array (
		0 => '~<h[123][^>]+>~si',
		1 => '~<h[456][^>]+>~si',
		2 => '~<table[^>]+>~si',
		3 => '~<tr[^>]+>~si',
		4 => '~<li[^>]+>~si',
		5 => '~<br[^>]+>~si',
		6 => '~<p[^>]+>~si',
		7 => '~<div[^>]+>~si',
		);		
		$html = preg_replace("/<p[^>]*?>/", "", $html);
		$html = str_replace("</p>", "\n", $html);
		$html = preg_replace($tags,"\n",$html);
		$html = preg_replace('~</t(d|h)>\s*<t(d|h)[^>]+>~si',' - ',$html);
		$html = preg_replace('~<[^>]+>~s','',$html);
		// reducing spaces
		$html = preg_replace('~ +~s',' ',$html);
		$html = preg_replace('~^\s+~m','',$html);
		$html = preg_replace('~\s+$~m','',$html);
		// reducing newlines
		$html = preg_replace('~\n+~s',"\n",$html);
		$html = str_replace('&nbsp;',"\n",$html);
		return $html;
}
