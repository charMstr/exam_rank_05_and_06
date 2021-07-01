/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BrickWall.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:09:11 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:33:18 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BrickWall.hpp"

//constructor
BrickWall::BrickWall(void) : ATarget("Inconspicuous Red-brick Wall")
{}

BrickWall *
BrickWall::clone(void) const
{
	return (new BrickWall());	
}
